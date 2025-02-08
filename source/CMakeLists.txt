################################################################################
# Compile each iAnt-ARGoS class into a shared object file.
################################################################################
add_library(foraging_nn_controller     SHARED foraging_nn_controller.h
                                    foraging_nn_controller.cpp
                                    foraging_nn_loop_functions.h
                                    foraging_nn_loop_functions.cpp
                                    )

add_library(foraging_nn_loop_functions SHARED foraging_nn_loop_functions.h
                                    foraging_nn_loop_functions.cpp
                                    foraging_nn_controller.h
                                    foraging_nn_controller.cpp
                                    foraging_nn_qt_user_functions.h
                                    foraging_nn_qt_user_functions.cpp
                                    )

# add_library(BasicGA SHARED          nn/Perceptron.cpp
#                                     nn/Perceptron.h
#                                     nn/Neuron.cpp nn/Neuron.h
#                                     nn/NeuralNetwork.cpp
#                                     nn/NeuralNetwork.h
#                                     ga/Chromosome.cpp
#                                     ga/Chromosome.h
#                                     nn/NeuralNetFactory.cpp
#                                     nn/NeuralNetFactory.h
#                                     nn/ValueNeuron.cpp
#                                     nn/ValueNeuron.h
#                                     ga/BasicGA.cpp
#                                     ga/BasicGA.h
#                                     ga/Unique.cpp
#                                     ga/Unique.h
#                                     ga/ChromosomeFactory.cpp
#                                     ga/ChromosomeFactory.h)

################################################################################
# Correctly link each shared object with its dependencies . . .
################################################################################

# add_executable(iant_main main.cpp)

target_link_libraries(foraging_nn_controller
                      argos3core_simulator
                      argos3plugin_simulator_entities
                      argos3plugin_simulator_footbot
                      argos3plugin_simulator_genericrobot)

target_link_libraries(foraging_nn_loop_functions
                      argos3core_simulator
                      argos3plugin_simulator_entities
                      argos3plugin_simulator_footbot
                      argos3plugin_simulator_genericrobot
                      argos3plugin_simulator_qtopengl)

target_link_libraries(
	    foraging_nn_loop_functions
        foraging_nn_controller
        argos3core_simulator
        )
