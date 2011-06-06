/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "HBridge.hpp"
#include "Simulation.h"

using namespace avalon_simulation;

HBridge::HBridge(std::string const& name, TaskCore::TaskState initial_state)
: HBridgeBase(name, initial_state)
{
    for(int i=0;i<8;i++)
        m_controlled_hbridges[i]=false;
}

HBridge::HBridge(std::string const& name, RTT::ExecutionEngine* engine, TaskCore::TaskState initial_state)
: HBridgeBase(name, engine, initial_state)
{
    for(int i=0;i<8;i++)
        m_controlled_hbridges[i]=false;
}

HBridge::~HBridge()
{
}

bool HBridge::dispatch(::std::string const & name, ::std::vector< boost::int32_t > const & actuators, bool read_only)
{
    // Verify name clashes
    if (provides()->hasService("cmd_" + name))
    {
        RTT::log(RTT::Error) << "cannot create a dispatch called '" << name << "' as one of the port names 'cmd_" << name << "', 'status_" << name << "', 'errors_" << name << "' are already in use in the task interface" << RTT::endlog();
        return false;
    }

    for (unsigned int i = 0; i < actuators.size(); ++i)
    {
        int board_idx = abs(actuators[i]) - 1;
        if (board_idx == -1)
        {
            // A zero board index means "fill in an empty data structure"
            continue;
        }
        else if (board_idx > BOARD_COUNT)
        {
            log(RTT::Error) << "invalid board ID " << board_idx << RTT::endlog();
            return false;
        }
        if (!read_only && m_controlled_hbridges[board_idx])
        {
            log(RTT::Error) << "cannot add " << name <<
                ": there is already someone controlling board " <<
                board_idx << RTT::endlog();
            return false;
        }
        m_controlled_hbridges[board_idx] = !read_only;
    }

    // Preallocate the data structure to avoid allocations later on (hard
    // realtime)
    //info.status.resize(actuators.size());
    provides()->addPort(* new OutputPortType("status_" + name));
    //    if (!read_only)
    //    {
    //info.input_port = new InputPortType("cmd_" + name);
    //provides()->addEventPort(*info.input_port);
    //info.cmd.resize(actuators.size());
    InputPortType *p = new InputPortType("cmd_" + name);
    _ports.push_back(p);
    provides()->addEventPort(*p);
    printf("Created Port cmd_%s\n",name.c_str());


    //info.output_port = new OutputPortType("status_" + name);
    //provides()->addPort(*info.output_port);
    //    }else{
    //    	printf("Not creating port because its readOnly\n");
    //    }

    //info.output_port = new OutputPortType("status_" + name);
    //provides()->addPort(*info.output_port);

    //provides()->addPort(*new OutputPortType("status_" + name));


    //info.error_port = new ErrorPortType("errors_" + name);
    //provides()->addPort(*info.error_port);
    // Initialize the connections with the preallocated data structure, so that
    // further write() are hard-realtime
    //info.output_port->setDataSample(info.status);
    // Finally, register the dispatch
    //m_dispatches.push_back(info);
    return bool();
}

/// The following lines are template definitions for the various state machine
// hooks defined by Orocos::RTT. See HBridge.hpp for more detailed
// documentation about them.

bool HBridge::configureHook()
{
    if (! HBridgeBase::configureHook())
        return false;
    return true;
}
bool HBridge::startHook()
{
    if (! HBridgeBase::startHook())
        return false;

    if(!Simulation::isInitialized())
        throw std::runtime_error("Cannot start Camera. The simulator is not running in the same process.");

    return true;
}
void HBridge::updateHook()
{
    HBridgeBase::updateHook();

    if (!Simulation::getSimulatorInterface()->isRunning()) 
        return;

    for (unsigned int dispatch_idx = 0; dispatch_idx < _ports.size(); dispatch_idx++)
    {
        //printf("dispatch id\n");
        InputPortType* port = _ports[dispatch_idx];
        base::actuators::Command cmd;
        double target[6];
        if(!port){
            printf("Invalid port is given\n");
        }
        if (port && port->readNewest(cmd) == RTT::NewData){
            //printf("new data\n");

            //todo make PWM<-> Newton/Meter configurable

            target[LEFT] = cmd.target[avalon_control::LEFT] * 10.0 * DIR_LEFT * avalon_control::DIR_LEFT;
            //printf("Setting forces: %f for %i\n",target[LEFT],LEFT);
            target[RIGHT] = cmd.target[avalon_control::RIGHT] * 10.0 * DIR_RIGHT * avalon_control::DIR_RIGHT;
            //printf("Setting forces: %f for %i\n",target[RIGHT],RIGHT);
            target[MIDDLE_VERTICAL] = cmd.target[avalon_control::MIDDLE_VERTICAL] * 10.0 * DIR_MIDDLE_VERTICAL * avalon_control::DIR_MIDDLE_VERTICAL;
            //printf("Setting forces: %f for %i\n",target[MIDDLE_VERTICAL], MIDDLE_VERTICAL);
            target[REAR_VERTICAL] = cmd.target[avalon_control::REAR_VERTICAL] * 10.0 * DIR_REAR_VERTICAL * avalon_control::DIR_REAR_VERTICAL;
            //printf("Setting forces: %f for %i\n",target[REAR_VERTICAL], REAR_VERTICAL);
            target[MIDDLE_HORIZONTAL] = cmd.target[avalon_control::MIDDLE_HORIZONTAL] * 10.0 * DIR_MIDDLE_HORIZONTAL * avalon_control::DIR_MIDDLE_HORIZONTAL;
            //printf("Setting forces: %f for %i\n",target[MIDDLE_HORIZONTAL], MIDDLE_HORIZONTAL);
            target[REAR_HORIZONTAL] = cmd.target[avalon_control::REAR_HORIZONTAL] * 10.0 * DIR_REAR_HORIZONTAL * avalon_control::DIR_REAR_HORIZONTAL;
            //printf("Setting forces: %f for %i\n",target[REAR_HORIZONTAL], REAR_HORIZONTAL);
            /*
               for (unsigned int j = 0; j < cmd.mode.size(); ++j)
               {
               base::actuators::DRIVE_MODE mode = cmd.mode[j];
               target[j] = cmd.target[j] * 10.0; //todo make PWM<-> Newton/Meter configurable
               printf("Setting forces: %f for %i\n",target[j],j);
               }
               */
            Simulation::getAvalonPlugin()->setForces(target);
        }
    }
}
// void HBridge::errorHook()
// {
//     HBridgeBase::errorHook();
// }
// void HBridge::stopHook()
// {
//     HBridgeBase::stopHook();
// }
// void HBridge::cleanupHook()
// {
//     HBridgeBase::cleanupHook();
// }
