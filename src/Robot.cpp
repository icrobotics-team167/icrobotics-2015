#include "Robot.h"
#include <iostream>

using std::cout;
using std::endl;

Robot::Robot()
{
        RealJoy1 = new Joystick(1);
        RealJoy2 = new Joystick(2);
        Joystick1 = new SimpleJoystick(RealJoy1);
        Joystick2 = new SimpleJoystick(RealJoy2);

        swerveWheels = new SwerveDrive(1988, 250, 0.00420921055, 4, 1, 0, 5, 6, 2, 3);
}

Robot::~Robot()
{
        delete RealJoy1;
        delete RealJoy2;
        delete Joystick1;
        delete Joystick2;
        delete swerveWheels;
}

void Robot::RobotInit()
{
	swerveWheels->ResetGyro();
	swerveWheels->ZeroRotateEncoders();
	swerveWheels->ZeroDriveEncoders();
}

void Robot::DisabledInit()
{
	swerveWheels->ResetGyro();
	swerveWheels->ZeroRotateEncoders();
	swerveWheels->ZeroDriveEncoders();

}

void Robot::DisabledPeriodic()
{

}

void Robot::AutonomousInit()
{

}

void Robot::AutonomousPeriodic()
{
}

void Robot::TeleopInit()
{
	swerveWheels->ResetGyro();
	swerveWheels->ZeroRotateEncoders();
	swerveWheels->ZeroDriveEncoders();
}

void Robot::TeleopPeriodic()
{
        JoystickOne();
}

void Robot::JoystickOne() {
        // Joy1 Control Code
        // get joystick position
        float x = this->RealJoy1->GetAxis(Joystick::kXAxis);
        float y = -this->RealJoy1->GetAxis(Joystick::kYAxis);
        float z = abs(Vector3::GetRotation(x, y) - 90);

        float abs_x = abs(x), abs_y = abs(y);

        if (abs_x < 0.1 && abs_y < 0.1)
        {
                swerveWheels->Stop();
                return;
        }

        // Set the Throttle
        bool turbo = Joystick1->Toggled(BUTTON_8);

        /*
         * raw axis 4 is the throttle axis on the Logitech Extreme 3D Pro joystick
         * we use the raw axis because the default mappings are incorrect
         * the throttle, by default, returns values from -1.0 at the plus position to 1.0 at the minus position
         * we first multiply by -1.0 to get values from -1.0 at the minus position to 1.0 at the plus position
         * we then add 1.0 and divide by 2 to get final voltage percentages from 0.0 (off) at minus position
         * to 1.0 (full throttle) at the plus position
         */
        double throttle_mag = (this->RealJoy1->GetThrottle() * -1.0 + 1.0) / 2.0;


        double speed = throttle_mag;

        if (!Joystick1->Pressed(BUTTON_5) && !Joystick1->Pressed(BUTTON_6))
        {
                        // if we are not turning get the larger of the x and y values of the joystick posistion,
                        // and multiply that by the throttle to get final voltage
                        speed *= std::max(abs_x, abs_y);
        }
        else
        {
                        // if we are turning, the rate of turning depends only on the throttle setting,
                        // and the rate of turning is limited to 80% voltage maximum
                        speed *= 0.8;
        }

        if (speed < 0.1)
                speed = 0.1;

        if (turbo)
                speed *= 1.5;

        //cout << "speed = " << speed << endl;

        if (speed > 1.0)
                speed = 1.0;

        //cout << "z = " << z << endl;
        swerveWheels->Drive(z, speed);
}

START_ROBOT_CLASS(Robot);
