/* Generated from orogen/lib/orogen/templates/tasks/Task.hpp */

#ifndef AVALON_SIMULATION_TASK_TASK_HPP
#define AVALON_SIMULATION_TASK_TASK_HPP

#include "avalon_simulation/TaskBase.hpp"
//#include <HBridgeDriver.hpp>
#define BOARD_COUNT 8

#include <mars/avalon-plugin/AvalonPlugin.h>

class AvalonPlugin;

namespace avalon_simulation {
    class Task : public TaskBase
    {
	friend class TaskBase;
    protected:
	AvalonPlugin *avalon;


        typedef RTT::InputPort< base::actuators::Command > InputPortType;
        std::vector<InputPortType*> _ports;
	//typedef RTT::OutputPort< hbridge::Error > ErrorPortType;
	/*
        struct DispatchInfo
        {
            std::string name;
            InputPortType* input_port;
            OutputPortType* output_port;
            ErrorPortType* error_port;
            std::vector<int> reverse;
            std::vector<int> boards;

            base::actuators::Status status;
            base::actuators::Command cmd;

            DispatchInfo()
                : input_port(0), output_port(0), error_port(0) {}
        };
	*/
        //std::vector<DispatchInfo> m_dispatches;

        /** m_used_hbridges[i] is true if there is at least one dispatch that
         * involves board 'i' and false otherwise
         */
	bool m_used_hbridges[BOARD_COUNT];
        /** m_controlled_hbridges[i] is true if there is at least one read-write
         * dispatch that involves board 'i'
         */
	bool m_controlled_hbridges[BOARD_COUNT];
        /** When reading the data, m_got_status[i] is true when we received one
         * status message from board 'i'. It gets reset when we push the data to
         * the output ports
         */
	bool m_got_status[BOARD_COUNT];
        /** If true, we have at least one board used in the 1-4 set */
        bool m_uses_board14;
        /** If true, we have at least one board used in the 5-8 set */
        bool m_uses_board58;
        /** When the component is running, this attributes contains the
         * first used board index
         */
        int m_master_board;
        /** The time of the last CAN update for the master board. It is always
         * used to timestamp the next status message
         */
        base::Time m_master_time;


    public:
        Task(std::string const& name = "avalon_simulation::Task");

        /** This hook is called by Orocos when the state machine transitions
         * from PreOperational to Stopped. If it returns false, then the
         * component will stay in PreOperational. Otherwise, it goes into
         * Stopped.
         *
         * It is meaningful only if the #needs_configuration has been specified
         * in the task context definition with (for example):
         *
         *   task_context "TaskName" do
         *     needs_configuration
         *     ...
         *   end
         */
        bool configureHook();

        /** This hook is called by Orocos when the state machine transitions
         * from Stopped to Running. If it returns false, then the component will
         * stay in Stopped. Otherwise, it goes into Running and updateHook()
         * will be called.
         */
        bool startHook();

        /** This hook is called by Orocos when the component is in the Running
         * state, at each activity step. Here, the activity gives the "ticks"
         * when the hook should be called.
         *
         * The error(), exception() and fatal() calls, when called in this hook,
         * allow to get into the associated RunTimeError, Exception and
         * FatalError states. 
         *
         * In the first case, updateHook() is still called, and recover() allows
         * you to go back into the Running state.  In the second case, the
         * errorHook() will be called instead of updateHook(). In Exception, the
         * component is stopped and recover() needs to be called before starting
         * it again. Finally, FatalError cannot be recovered.
         */
        void updateHook();

        /** This hook is called by Orocos when the component is in the
         * RunTimeError state, at each activity step. See the discussion in
         * updateHook() about triggering options.
         *
         * Call recovered() to go back in the Runtime state.
         */
        // void errorHook();

        /** This hook is called by Orocos when the state machine transitions
         * from Running to Stopped after stop() has been called.
         */
        // void stopHook();

        /** This hook is called by Orocos when the state machine transitions
         * from Stopped to PreOperational, requiring the call to configureHook()
         * before calling start() again.
         */
        // void cleanupHook();
        
	virtual bool dispatch(::std::string const & name, ::std::vector< ::boost::int32_t > const & actuators, bool read_only);
    };
}

#endif
