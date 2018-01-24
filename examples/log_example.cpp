#include <XBotLogger/Logger.hpp>

#include <unistd.h>


int main(int argc, char **argv){

    using XBot::MatLogger;
    
    /* Create matlogger */
    auto logger = MatLogger::getLogger("/tmp/example_log");
    
    /* Log some useless data streams */
    std::vector<double> std_values;
    Eigen::VectorXd eigen_values;
    Eigen::MatrixXd eigen_values_matrix;
    double scalar_value;
    
    for(int i = 0; i < 100; i++)
    {
        std_values.assign(10, std::sin(2*M_PI*i/20));
        eigen_values.setRandom(15);
        eigen_values_matrix.setRandom(4,4);
        scalar_value = std::sin(2*M_PI*i/20);
        
        logger->add("std_values", std_values);
        logger->add("eigen_values", eigen_values);
        logger->add("eigen_values_matrix", eigen_values_matrix);
        logger->add("scalar_value", scalar_value);
    }
    
    /* Save one giant matrix */
    eigen_values_matrix.setRandom(30, 70);
    logger->log("giant_matrix", eigen_values_matrix);
    
    /* Save to disk */
    logger->flush();

}
