/*
 * SwerveDrive.cpp
 *
 *  Created on: Jan 8, 2015
 *      Author: Kazimier
 */

#include <iostream>
#include "SwerveDrive.h"

using std::cout;
using std::endl;

SwerveDrive::SwerveDrive(unsigned rotateEncLines, unsigned driveEncLines,
                        unsigned feetToEncLinesR,
                        unsigned short rotateTalon1Number,
                        unsigned short rotateTalon2Number,
                        unsigned short gyroChannel,
                        int talon1Number,
                        int talon2Number,
                        int talon3Number,
                        int talon4Number)
{
        rotateEncoderLines = rotateEncLines;
        driveEncoderLines = driveEncLines;
        feetToEncoderLinesRatio = feetToEncLinesR;

        rotateTalon1 = new CANTalon(rotateTalon1Number);
        rotateTalon2 = new CANTalon(rotateTalon2Number);

		cout << "rotateTalon1 initial encoder position = " << rotateTalon1->GetEncPosition() << endl;

        talon1 = new CANTalon(talon1Number);
        talon2 = new CANTalon(talon2Number);
        talon3 = new CANTalon(talon3Number);
        talon4 = new CANTalon(talon4Number);

        gyro = new Gyro(gyroChannel);
        gyro->InitGyro();

        driveDistance = 0;
        finished = true;
}

/*
 * talon1--talon3
 * |			|
 * |			|
 * talon2--talon4
 */
SwerveDrive::~SwerveDrive()
{
        delete rotateTalon1;
        delete rotateTalon2;
        delete gyro;
        delete talon1;
        delete talon2;
        delete talon3;
        delete talon4;
}

/*
 * angle: the direction relative to the field in which the
 * robot should drive; field forward is 0/360 degrees
 *
 * speed: double ranging from -1.0 to 1.0 which represents the speed at
 * which the wheels should be driven; -1.0 is full reverse, 1.0 is full forward
 */
void SwerveDrive::Drive(int angle, double speed)
{
		cout << "rotateTalon1 encoder position = " << rotateTalon1->GetEncPosition() << endl;

		RotateWheelsOnce();
		return;

        if (angle == -1 || speed == 0) {
                talon1->Set(0);
                talon2->Set(0);
                talon3->Set(0);
                talon4->Set(0);
                rotateTalon1->Set(0);
                rotateTalon2->Set(0);
                return;
        } // If they joystick is in neutral position or the speed is zero, do nothing.

        SwerveState swerveState = TurnRobot(angle);
        if (swerveState != DRIVE_NOT)
        {
        		speed *= ((swerveState == DRIVE_FORWARDS) ? 1 : -1);
                rotateTalon1->Set(0);
                rotateTalon2->Set(0);
                talon1->Set(speed);
                talon2->Set(speed);
                talon3->Set(speed);
                talon4->Set(speed);
        }	//stops the motors when the wheels are pointing the right way
                //and tells the drive motors to spin

}

void SwerveDrive::Stop()
{
        Drive(-1, 0);
}

SwerveDrive::SwerveState SwerveDrive::TurnRobot(int angle)
{
		//cout << "GetWheelAngle() = " << GetWheelAngle() << endl;
		//cout << "rotateTalon1 encoder position = " << rotateTalon1->GetEncPosition() << endl;
		//cout << "gyro angle = " << gyro->GetAngle() << endl;
		SwerveState returnValue = DRIVE_FORWARDS;

        // always rotate the wheels at maximum speed
        double rotateWheelSpeed = 1.0;
        //we assume that when (rotateWheelSpeed == 1.0) the wheels will
        //rotate in a counterclockwise direction

        int angleToBeTurned = 0;

        if (angle > GetWheelAngle())
                angleToBeTurned = (angle - GetWheelAngle()) % 360;
        else
                angleToBeTurned = (GetWheelAngle() - angle) % 360;

        if (abs(angleToBeTurned - angle) % 360 > 90)
        {
                if ((abs(angleToBeTurned - angle) % 360) > 270)
                        //makes wheels rotate towards the original angle if they're closer to that
                                rotateWheelSpeed *= -1;
                else	//if the wheels are closer to (180 + angle) than (angle), they turn to (180 + angle)
                {
                        angleToBeTurned = -(180 - angleToBeTurned) % 360;
                        //finds the opposite of the angle needed to be turned relative to the robot
                        if (angleToBeTurned < 0)
                        {
                                angleToBeTurned *= -1;
                                returnValue = DRIVE_BACKWARDS;
                        }//makes sure wheels rotate the shortest distance
                }
        } //the if statement means the wheels won't have to turn over 90 degrees


        int distance = angleToBeTurned * rotateEncoderLines / 360;

        int currentPosition = ConvertEncoderValue();
        int targetPosition = distance % rotateEncoderLines;
        cout << "currentPosition = " << currentPosition << endl;
        cout << "targetPosition = " << targetPosition << endl;

        if (currentPosition > (targetPosition + ENCODER_ERROR_AMOUNT) || currentPosition < (targetPosition - ENCODER_ERROR_AMOUNT))
        {//the if statement doesn't use == because that level of precision is practically unattainable for the 'bot
                rotateTalon1->Set(rotateWheelSpeed);
                rotateTalon2->Set(rotateWheelSpeed);
                talon1->Set(0);
                talon2->Set(0);
                talon3->Set(0);
                talon4->Set(0);
                return DRIVE_NOT;
        }	//spins the motors until the wheels point in the right direction and stops drive motors

        else
                return returnValue;
}

bool SwerveDrive::DriveACertainDistance(double feet, double speed)
{
        if (driveDistance == 0 && finished)
                        driveDistance = feet / feetToEncoderLinesRatio;

        finished = false;

        int startPosition = talon1->GetEncPosition();

        talon1->Set(speed);
        talon2->Set(speed);
        talon3->Set(speed);
        talon4->Set(speed);

        int endPosition = talon1->GetEncPosition();

        driveDistance = driveDistance - abs(endPosition - startPosition);
        if (driveDistance <= 0)
        {
                finished = true;
                driveDistance = 0;
        }

        return finished;
}

void SwerveDrive::RotateRobot(bool clockwise, double speed)
{ // Rotate the robot in a given direction by speed 'speed'
        if (clockwise) {
                talon1->Set(speed);
                talon2->Set(speed);
                talon3->Set(-speed);
                talon4->Set(-speed);
        }
        else {
                talon1->Set(-speed);
                talon2->Set(-speed);
                talon3->Set(speed);
                talon4->Set(speed);
        }
}

int SwerveDrive::GetWheelAngle()
{
        int gyroangle = (int) gyro->GetAngle() % 360;
        int robotRelativeWheelAngle = rotateTalon1->GetEncPosition() * 360 / rotateEncoderLines;
        int fieldRelativeWheelAngle = ((int)gyroangle + robotRelativeWheelAngle) % 360;
        return fieldRelativeWheelAngle;
}	//finds the angle the wheels are facing relative to the field

int SwerveDrive::GetGyroAngle()
{
	return (int) (gyro->GetAngle()) % 360;
}

int SwerveDrive::ConvertEncoderValue()
{
	int moddedValue = talon1->GetEncPosition() % rotateEncoderLines;
	if (0 > moddedValue)
		moddedValue = rotateEncoderLines + moddedValue;
	return moddedValue;
}

void SwerveDrive::RotateWheelsOnce()
{
	if (ConvertEncoderValue() > (rotateEncoderLines / 2 + ENCODER_ERROR_AMOUNT) || ConvertEncoderValue() < (rotateEncoderLines / 2 - ENCODER_ERROR_AMOUNT))
	{
		rotateTalon1->Set(1);
		rotateTalon2->Set(1);
	}
	else
	{
		rotateTalon1->Set(0);
		rotateTalon2->Set(0);
	}
}
