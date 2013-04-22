/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "Microphones.hpp"
#include <simulation/MarsPlugin.hpp>
#include <mars/sim/SimNode.h>
#include <mars/sim/NodeManager.h>
#include <base/pose.h>
#include "Simulation.h"
#include <math.h>


using namespace avalon_simulation;

struct avalon_simulation::MicrophonePlugin: public simulation::MarsPlugin
{
    int node_id;
    Eigen::Quaterniond orientation;
    Eigen::Vector3d position;

    MicrophonePlugin()
        : node_id(0)
    {}

    void update( double time )
    {
        //mars::sim::SimNode *node = control->nodes->getSimNode(node_id);
        orientation = control->nodes->getRotation(node_id);
        position = control->nodes->getPosition(node_id);
    }

    void setMotorName( const std::string& name )
    {
        node_id = control->nodes->getID( name );
        if( !node_id)
            throw std::runtime_error("There is no Node by the name of " + name + " in the scene");
    }

    Eigen::Vector3d getPosition(){
        return position;
    }
    
    Eigen::Quaterniond getOrientation(){
        return orientation;
    }


};


Microphones::Microphones(std::string const& name, TaskCore::TaskState initial_state)
    : MicrophonesBase(name, initial_state)
{
}

Microphones::Microphones(std::string const& name, RTT::ExecutionEngine* engine, TaskCore::TaskState initial_state)
    : MicrophonesBase(name, engine, initial_state)
{
}

Microphones::~Microphones()
{
}



/// The following lines are template definitions for the various state machine
// hooks defined by Orocos::RTT. See Microphones.hpp for more detailed
// documentation about them.




bool Microphones::configureHook()
{
    
    if (! RTT::TaskContext::configureHook())
        return false;
    

    

    
    return true;
    
}



bool Microphones::startHook()
{
    
    if (! RTT::TaskContext::startHook())
        return false;
    
    mp= new MicrophonePlugin();
    mp->setMotorName( _node_name.value() );


    sample.sample_frequency = _sample_rate;
    sample.timestamp = base::Time::now();
    sample.left_channel.resize((int) _sample_rate);
    sample.right_channel.resize((int) _sample_rate);
    
    pingSample.sample_frequency = _sample_rate;
    pingSample.timestamp = base::Time::now();
    pingSample.left_channel.resize((int) ((_sample_rate / 100)+50));
    pingSample.right_channel.resize((int) ((_sample_rate / 100)+50) );

    return true;
    
}



void Microphones::updateHook()
{

	MicrophonesBase::updateHook();
	Eigen::Vector3d wPos = mp->getPosition();
	Eigen::Quaterniond  orient = mp->getOrientation();
	double c = _base_line/2;
	double a,b,c1, c2, alpha, beta;
	int diff;
	Eigen::Quaterniond rotation = base::Quaterniond(Eigen::AngleAxisd(-(base::getYaw(orient)),Eigen::Vector3d::UnitZ()));
	Eigen::Vector3d relPos1, relPos2, ASVpos;
	Eigen::Quaterniond quarter;

	Simulation::getAvalonPlugin()->getASVPose(ASVpos,quarter);

	relPos1(0) = 0+c;
	relPos1(1) = 0;
	relPos1(2) = 0;

	relPos2(0) = 0-c;
	relPos2(1) = 0;
	relPos2(2) = 0;

	relPos1 = (rotation.conjugate()*relPos1) + wPos;
	relPos2 = (rotation.conjugate()*relPos2) + wPos;


//	alpha = M_PI - yaw;
//	beta = (M_PI/2) - alpha;
//	a = c * cos(beta);
//	b = sqrt(pow(c,2) - pow(a,2));
	




	
	std::cerr << "x1= " << relPos1(0) << " y1= " << relPos1(1) << std::endl;
	std::cerr << "x2= " << relPos2(0) << " y2= " << relPos2(1) << std::endl << std::endl;


	relPos1 = ASVpos - relPos1;
	relPos2 = ASVpos - relPos2;


	std::cerr << "Rel1= " << relPos1.norm() << std::endl;
	std::cerr << "Rel2= " << relPos2.norm() <<  std::endl << std::endl;





	diff = (int)(_sample_rate * ((relPos2.norm() - relPos1.norm())/_sound_velocity));
	


	for(int i=0; i<sample.left_channel.size(); i++){
		sample.right_channel[i] = 0;
		sample.left_channel[i] = 0;
	}


	if (diff >0.0){

		for(int i=0; i<(sample.left_channel.size()/100)-4; i+=4){
			sample.left_channel[i] = 1;
			sample.left_channel[i+2] = -1;


		}

		for(int i=diff; i<(sample.right_channel.size()/100)-4+diff; i+=4){
			sample.right_channel[i] = 1;
			sample.right_channel[i+2] = -1;
		}
	}
	else if(diff < 0.0){

		for(int i=abs(diff); i<sample.left_channel.size()/100-4+abs(diff); i+=4){
			sample.left_channel[i] = 1;
			sample.left_channel[i+2] = -1;
		}

		for(int i=0; i<(sample.right_channel.size()/100)-4; i+=4){
			sample.right_channel[i] = 1;
			sample.right_channel[i+2] = -1;
		}
	}
	else{
		for(int i=0; i<(sample.right_channel.size()/100)-4; i+=4){
					sample.right_channel[i] = 1;
					sample.right_channel[i+2] = -1;

					sample.left_channel[i] = 1;
					sample.left_channel[i+2] = -1;
				}
	}


	for(int i=0; i<pingSample.left_channel.size(); i++){
			pingSample.left_channel[i] = sample.left_channel[i];
			pingSample.right_channel[i] = sample.right_channel[i];
		}



	_ping_output.write(pingSample);

	_stereo_output.write(sample);

    RTT::TaskContext::updateHook();
}



void Microphones::errorHook()
{
    
    RTT::TaskContext::errorHook();
}



void Microphones::stopHook()
{
    delete mp;
    RTT::TaskContext::stopHook();
}



void Microphones::cleanupHook()
{
    
    RTT::TaskContext::cleanupHook();
}
