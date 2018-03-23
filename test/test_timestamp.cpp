#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE "test_timestamp"
#define BOOST_AUTO_TEST_MAIN

#include <iostream>
#include <numeric>

#include <boost/test/unit_test.hpp>
#include <boost/test/execution_monitor.hpp>

#include <stream_aligner/TimestampEstimator.hpp>
#include <fstream>
#include <iomanip>


/**
 * helper class for unit testing This class calculates the sample time, hardware
 * time and real time for a given sample number.
 *
 * It also provides some functionallity to create plottable data and performs
 * some standard unit testing.
 * */
class Tester
{
private:
    std::ofstream debugFile;
public:
    /** static latency of the sample **/
    base::Time sample_latency;

    /** maximum of random noise added ontop of the static latency **/
    base::Time sample_latency_max_noise;

    /** maximum noise of hardware timestamp **/
    base::Time hw_time_max_noise;

    /** time of the first sample **/
    base::Time base_time;

    /** real period **/
    base::Time real_period;

    /** Drift of the period in s/s
    I.e. it is the rate of change of period in seconds per second **/
    base::Time period_drift;

    /** output, calculated by classe **/
    base::Time sample_time;

    /** output, calculated by classe **/
    base::Time hw_time;

    /** output, calculated by classe **/
    base::Time real_time;

    /** output: real_period with drift, calculated in update **/
    base::Time actual_period;

public:
    Tester(std::string debugFileName = "")
    {

        /** init random nummer generator **/
        srand48(time(NULL));

        base_time = base::Time::now();

        if(!debugFileName.empty())
            debugFile.open(debugFileName.c_str(), std::ios::out);

        if(debugFile.is_open())
        {
            debugFile << std::setprecision(5) << std::endl;
	        debugFile << "# (hardware-real) (sample-real) (estimate-real) (estimated_period-actual_period) (est-last_est) (real-last_real) period" << std::endl;
        }

    }

    void calculateSamples(int nr)
    {
        base::Time sample_latency_max_noise;
        if (nr > 0)
            sample_latency_max_noise = this->sample_latency_max_noise;
        else
            sample_latency_max_noise = this->real_period * 0.09;

        base::Time sample_latency_noise = base::Time::fromSeconds(drand48() * this->sample_latency_max_noise.toSeconds());

	    base::Time hw_time_noise(base::Time::fromSeconds(drand48() * this->hw_time_max_noise.toSeconds()));

        this->actual_period = this->real_period + this->period_drift * nr;
        this->real_time = this->base_time + this->real_period * nr + this->period_drift * nr * (nr + 1) / 2;
        this->sample_time = this->real_time + this->sample_latency + sample_latency_noise;
        this->hw_time = this->real_time + hw_time_noise;
    }

    void addResultToPlot(base::Time estimated_time, base::Time estimated_period)
    {
        static base::Time last_estimated_time = estimated_time;
        static base::Time last_real_time = real_time;

    	if(debugFile.is_open())
        {
	        debugFile << (hw_time - real_time).toSeconds() / actual_period.toSeconds()
                << " " << (sample_time - real_time - sample_latency).toSeconds() / actual_period.toSeconds()
                << " " << (estimated_time - real_time).toSeconds() / actual_period.toSeconds()
                << " " << (estimated_period - actual_period).toSeconds() / actual_period.toSeconds()
                << " " << (estimated_time - last_estimated_time).toSeconds()
                << " " << (real_time - last_real_time).toSeconds()
                << " " << actual_period.toSeconds()
                << std::endl; 
        }
        last_estimated_time = estimated_time;
        last_real_time = real_time;
    }

    void checkResult(base::Time estimated_time, base::Time estimated_period)
    {
        addResultToPlot(estimated_time, estimated_period);

        if ((std::abs((estimated_time - real_time).toSeconds()) > actual_period.toSeconds() / 10))
        {
            std::cout<<"estimated time: "<<estimated_time.toString()<<" real time: "<<real_time.toString()<<"\n";
        }

        BOOST_CHECK_SMALL(std::abs((estimated_time - real_time).toSeconds()), actual_period.toSeconds() / 10);
    }
};

/** type of announcement methods **/
enum LOSS_ANNOUNCE_METHODS
{
    USE_NONE,
    USE_UPDATE_LOSS,
    USE_INDEX
};


void test_timestamper_impl(int hardware_order, bool has_initial_period, bool has_drift, int init, double loss_rate, LOSS_ANNOUNCE_METHODS loss_announce_method = USE_NONE)
{
    /** this test case tries to emulate the values of an hokuyo laser scanner **/
    static const int COUNT = 10000;

    std::stringstream csv_filename;
    csv_filename << "test_timestamper";
    if (hardware_order < 0)
        csv_filename << "__hw_before";
    else if (hardware_order > 0)
        csv_filename << "__hw_after";
    if (has_initial_period)
        csv_filename << "__initial_period";
    if (has_drift)
        csv_filename << "__period_drift";
    if (init)
        csv_filename << "__init" << init;
    if (loss_rate)
        csv_filename << "__loss";
    csv_filename << ".csv";

    Tester data(csv_filename.str());
    data.real_period = base::Time::fromSeconds(0.025);

    if (hardware_order != 0)
    {
        data.sample_latency = base::Time::fromSeconds(0.02);
        if (init < 900)
            init = 900;
    }

    if (has_drift)
    {
        /** experimental value. In these tests, the filter does not cope anymore
        if the period drift is higher than that **/
        data.period_drift = base::Time::fromSeconds(3e-5);
    }
    data.sample_latency_max_noise = base::Time::fromSeconds(0.005);
    data.hw_time_max_noise = base::Time::fromMicroseconds(50);

    base::Time initial_period;
    if (has_initial_period)
        initial_period = base::Time::fromSeconds(0.025);

    /** estimator for testing **/
    stream_aligner::TimestampConfig config(base::Time::fromSeconds(20), initial_period);
    stream_aligner::TimestampEstimator estimator(config);

    std::cout<<"== START == "<<estimator.getStatus()<<"\n";

    for (int i = 0; i < COUNT; ++i)
    {
	    data.calculateSamples(i);

        if (hardware_order < 0)
            estimator.updateReference(data.hw_time);

        if (drand48() < loss_rate)
        {
            if (loss_announce_method == USE_UPDATE_LOSS)
                estimator.updateLoss();
            continue;
        }

        base::Time estimated_time;
        if (loss_announce_method == USE_INDEX)
            estimated_time = estimator.update(data.sample_time, i);
        else
            estimated_time = estimator.update(data.sample_time);

        if (hardware_order > 0)
            estimator.updateReference(data.hw_time);

        base::Time period = estimator.getPeriod();

        if (i < init)
            data.addResultToPlot(estimated_time, period);
        else
            data.checkResult(estimated_time, period);
    }
    std::cout<<"== END == "<<estimator.getStatus()<<"\n";
};


BOOST_AUTO_TEST_CASE(test_perfect_stream)
{
    base::Time time = base::Time::now();
    base::Time step = base::Time::fromSeconds(0.01);

    stream_aligner::TimestampConfig config(base::Time::fromSeconds(2), base::Time::fromSeconds(0));
    stream_aligner::TimestampEstimator estimator(config);
    for (int i = 0; i < 10000; ++i)
    {
        time = time + step;
        std::cout<<"time: "<<time.toString()<<"\n";
        BOOST_REQUIRE_CLOSE(time.toSeconds(), estimator.update(time).toSeconds(), 0.0000001);
        BOOST_REQUIRE_EQUAL(0, estimator.getLostSampleCount());
    }

    std::cout<<"estimated period:" <<estimator.getPeriod().toSeconds() <<"\n";
    BOOST_REQUIRE_CLOSE(step.toSeconds(), estimator.getPeriod().toSeconds(), 1e-6);

    std::cout<< estimator.getStatus() <<std::endl;
}

BOOST_AUTO_TEST_CASE(test_timestamper__plain)
{ test_timestamper_impl(0, false, false, 1000, 0); }

BOOST_AUTO_TEST_CASE(test_timestamper__hw_before__initial_period)
{ test_timestamper_impl(-1, true, false, 0, 0); }


