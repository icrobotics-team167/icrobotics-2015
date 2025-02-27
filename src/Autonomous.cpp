/*
 * Autonomous.cpp
 *
 *  Created on: Jan 22, 2015
 *      Author: moon_gecko
 */

#include "Robot.h"

void Robot::PickUpTote()
{
        elevatorArms->Open();
        bool done = swerveWheels->DriveACertainDistance(2.0, 1.0);
        if (done)
        {
                elevatorArms->Close();
                autoState = DRIVE_FORWARD;
        }
}

void Robot::Stop()
{
        swerveWheels->Stop();
}

void Robot::DriveForward()
{
        bool check = swerveWheels->DriveACertainDistance(8.92, 1.0); //drive 8.92 feet and at speed 1.0 (full speed)
        if (check) { autoState = STOP; }
}

void Robot::StackBin ()
{

}
