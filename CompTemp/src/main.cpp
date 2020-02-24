#define _USE_MATH_DEFINES

#include "vex.h"
#include <cmath>
#include "MiniPID.h"

using namespace vex;  
double driveSpeedFactor = 1.3;
int barPos = 0;
int auton = -1;
int allSpeed = 100;
int intake = 100;
int mode = 2;
std::string driveType = "Hybrid Controls";
std::string driveSpeed = "Drive Speed Normal"; 
double degreesPerFoot = 340.0;
double degreesTo360 = degreesPerFoot * 4;

competition Competition;
brain Brain;
motor RightFrontMotor = motor(PORT17, ratio18_1, true);
motor RightRearMotor = motor(PORT20, ratio18_1, true);
motor LeftRearMotor = motor(PORT19, ratio18_1, false);
motor LeftFrontMotor = motor(PORT14, ratio18_1, false);
motor LeftIntakeMotor = motor(PORT11, ratio18_1, false);
motor RightIntakeMotor = motor(PORT12, ratio18_1, true);
motor TilterMotor = motor(PORT18, ratio36_1, true);
motor BarMotor = motor(PORT13, ratio36_1, false);
pot Poten = pot(Brain.ThreeWirePort.H);
inertial Inertial = inertial(PORT21);
motor_group LeftSide = motor_group(LeftFrontMotor, LeftRearMotor);
motor_group RightSide = motor_group(RightFrontMotor, RightRearMotor);
motor_group Intakes = motor_group(LeftIntakeMotor, RightIntakeMotor);
drivetrain Drivetrain = drivetrain(LeftSide, RightSide, 12.56, 13, 9.5, distanceUnits::in, 1.0);
smartdrive SmartDrive = smartdrive(LeftSide, RightSide, Inertial, 12.56, 13, 9.5, distanceUnits::in, 1.0);
controller Controller1;
MiniPID tilt = MiniPID(1, 0, 0);

extern void askPosition() {
  Brain.Screen.clearScreen();
  Brain.Screen.setFont(prop30);
  Brain.Screen.setPenColor(color(180, 180, 180));
  Brain.Screen.printAt(0, 30, "Battery Percentage: %d%%", Brain.Battery.capacity());
  Brain.Screen.setFont(prop20);
  Brain.Screen.drawLine(300, 0, 300, 40);
  Brain.Screen.setPenColor(color(0, 150, 0));
  Brain.Screen.drawRectangle(305, 0, 170, 40, color(0, 150, 0));
  Brain.Screen.setPenColor(black);
  Brain.Screen.printAt(365, 25, false, "Skills");
  Brain.Screen.setPenColor(color(180, 180, 180));
  Brain.Screen.setFont(prop30);
  Brain.Screen.drawLine(0, 40, 480, 40);
  Brain.Screen.setPenColor(color(180, 0, 0));
  Brain.Screen.drawRectangle(10, 50, 220, 85, color(180, 0, 0));
  Brain.Screen.setPenColor(black);
  Brain.Screen.printAt(95, 100, false, "RFS");
  Brain.Screen.setPenColor(color(0, 0, 180));
  Brain.Screen.drawRectangle(250, 50, 220, 85, color(0, 0, 180));
  Brain.Screen.setPenColor(black);
  Brain.Screen.printAt(335, 100, false, "BFS");
  Brain.Screen.setPenColor(color(180, 0, 0));
  Brain.Screen.drawRectangle(10, 145, 220, 85, color(180, 0, 0));
  Brain.Screen.setPenColor(black);
  Brain.Screen.printAt(95, 195, false, "RBS");
  Brain.Screen.setPenColor(color(0, 0, 180));
  Brain.Screen.drawRectangle(250, 145, 220, 85, color(0, 0, 180));
  Brain.Screen.setPenColor(black);
  Brain.Screen.printAt(335, 195, false, "BBS");
  Brain.Screen.setPenColor(color(180, 180, 180));
}

extern void pre_auton(void) {
  int xLastTouch = Brain.Screen.xPosition();
  int yLastTouch = Brain.Screen.yPosition();
  askPosition();
  while (Brain.Screen.xPosition() == xLastTouch && Brain.Screen.yPosition() == yLastTouch) {
    vex::task::sleep(20);
  }
  bool k = true;
  while (k) {
    if ((Brain.Screen.xPosition() >= 10 && Brain.Screen.xPosition() <= 230) && (Brain.Screen.yPosition() >= 50 && Brain.Screen.yPosition() <= 135)) {
      auton = 0;
      k = false;
    } else if ((Brain.Screen.xPosition() >= 250 && Brain.Screen.xPosition() <= 470) && (Brain.Screen.yPosition() >= 50 && Brain.Screen.yPosition() <= 135)) {
      auton = 1;
      k = false;
    } else if ((Brain.Screen.xPosition() >= 10 && Brain.Screen.xPosition() <= 230) && (Brain.Screen.yPosition() >= 145 && Brain.Screen.yPosition() <= 230)) {
      auton = 2;
      k = false;
    } else if ((Brain.Screen.xPosition() >= 250 && Brain.Screen.xPosition() <= 470) && (Brain.Screen.yPosition() >= 145 && Brain.Screen.yPosition() <= 230)) {
      auton = 3;
      k = false;
    } else if ((Brain.Screen.xPosition() >= 305 && Brain.Screen.xPosition() <= 475) && (Brain.Screen.yPosition() >= 0 && Brain.Screen.yPosition() <= 40)) {
      auton = 4;
      k = false;
    }
  }

  if (auton == 0) {
    Brain.Screen.clearScreen();
    Brain.Screen.print("RFS Selected");
    Controller1.Screen.clearScreen();
    Controller1.Screen.print("RFS Selected");
  } else if (auton == 1) {
    Brain.Screen.clearScreen();
    Brain.Screen.print("BFS Selected");
    Controller1.Screen.clearScreen();
    Controller1.Screen.print("BFS Selected");
  } else if (auton == 2) {
    Brain.Screen.clearScreen();
    Brain.Screen.print("RBS Selected");
    Controller1.Screen.clearScreen();
    Controller1.Screen.print("RBS Selected");
  } else if (auton == 3) {
    Brain.Screen.clearScreen();
    Brain.Screen.print("BBS Selected");
    Controller1.Screen.clearScreen();
    Controller1.Screen.print("BBS Selected");
  } else if (auton == 4) {
    Brain.Screen.clearScreen();
    Brain.Screen.print("Skills Selected");
    Controller1.Screen.clearScreen();
    Controller1.Screen.print("Skills Selected");
  }

  Inertial.calibrate(2000);
  vex::task::sleep(2000);
}

void deployStack() {
  TilterMotor.setBrake(brake);
  double error = 2050;
  double tilt_speed = 100;
  while (std::abs(error) > 10 && std::abs(tilt_speed) > 5) {
    error = std::abs(2050-Poten.angle(rotationUnits::raw));
    tilt_speed = error/15;
    TilterMotor.spin(fwd, tilt_speed, pct);
    if (tilt_speed < 5) {
      break;
    }
  }
}

// void deployStack() {
//   TilterMotor.setBrake(brake);
//   double error = 2000-Poten.angle(rotationUnits::raw);
//   double tilt_speed = 100;
//   while (std::abs(error) > 10 && std::abs(tilt_speed) > 5) {
//     error = sqrt(2000-Poten.angle(rotationUnits::raw));
//     tilt_speed = error;
//     TilterMotor.spin(fwd, tilt_speed, pct);
//   }
// }

void flipOut() {
  TilterMotor.setVelocity(100, pct);
  Intakes.setVelocity(100, pct);
  TilterMotor.spinFor(fwd, 445, rotationUnits::deg);
  wait(200, msec);  
  Intakes.spinFor(directionType::rev, 1.2575, rotationUnits::rev);
  wait(200, msec);
  TilterMotor.spinFor(directionType::rev, 340, rotationUnits::deg);
  // BarMotor.spinFor(fwd, 900, deg);
  // TilterMotor.spinFor(fwd, 475, deg);
  // BarMotor.spinFor(directionType::rev, 900, deg);
  // Intakes.spinFor(directionType::rev, 2, rotationUnits::rev);
  // wait(50, msec);
  // TilterMotor.spinFor(directionType::rev, 450, deg);
  // BarMotor.spinFor(fwd, 400, deg);
  // wait(1, msec);
  // BarMotor.spinFor(directionType::rev, 400, deg);
}

// void turn(double degrees) {
//   if (Inertial.installed()) {
//     double target = degrees;
//     double error = target - Inertial.rotation();
//     double kP = .5;
//     if (error > 0) {
//       while (std::abs(error) > 1) {
//         error = target - Inertial.rotation();
//         double percent = kP * error + 15;
//         LeftSide.spin(directionType::fwd, percent, pct);
// 	      RightSide.spin(directionType::rev, percent, pct);
//         vex::task::sleep(20);
//       }
//     } else if (error > 0) {
//       double target = degrees; 
//       double error = target - Inertial.rotation();
//       double kP = .5;
//       while (std::abs(error) > 1) {
//         error = target - Inertial.rotation();
//         double percent = kP * error - 15;
//         LeftSide.spin(directionType::fwd, percent, pct);
// 	      RightSide.spin(directionType::rev, percent, pct);
//         vex::task::sleep(20);
//       }
//     } else {
//       LeftSide.stop();
//       RightSide.stop();
//       goto end;
//     }
//   } else {
//     Brain.Screen.clearScreen();
//     Brain.Screen.setFont(fontType::mono40);
//     Brain.Screen.setFillColor(red);
//     Brain.Screen.print("No Inertial Sensor Installed");
//   }
//   end:
//   vex::task::sleep(10);
// }

void turnLeft(double deg) {
  double error = 100;
  double kP = 0.5;
  double speed = 0;
  double ang = -1*deg;
  if (ang >= -180) {
    while (Inertial.rotation() > ang) {
      error = std::abs(ang - Inertial.rotation());
      speed = error*kP;
      RightSide.spin(fwd, speed, pct);
      LeftSide.spin(directionType::rev, speed, pct);
      if (error < 5) {
        break;
      }
    }
    RightSide.stop();
    LeftSide.stop();
  }
}

void turnRight(double deg) {
  double error = 100;
  double kP = 0.5;
  double speed = 0;
  double ang = deg;
  if (ang <= 180) {
    while (Inertial.rotation() < ang) {
      error = std::abs(ang - Inertial.rotation());
      speed = error*kP;
      RightSide.spin(directionType::rev, speed, pct);
      LeftSide.spin(fwd, speed, pct);
      if (error < 5) {
        break;
      }
    }
    RightSide.stop();
    LeftSide.stop();
  }
}

void stop_robot(){
    RightSide.stop();
    LeftSide.stop();
    RightSide.resetRotation();
    LeftSide.resetRotation();
}

void drive(double inches, double intRot){
  SmartDrive.driveFor(fwd, inches, distanceUnits::in, false);
  Intakes.spinFor(fwd, intRot, rotationUnits::rev);
  Intakes.stop();
}

void moveForward(double feet, double speed){
    double degrees = feet * degreesPerFoot;
    RightFrontMotor.startRotateFor(degrees, vex::rotationUnits::deg, speed, vex::velocityUnits::rpm);
    LeftFrontMotor.startRotateFor(degrees, vex::rotationUnits::deg, speed, vex::velocityUnits::rpm);
    RightRearMotor.startRotateFor(degrees, vex::rotationUnits::deg, speed, vex::velocityUnits::rpm);
    LeftRearMotor.rotateFor(degrees, vex::rotationUnits::deg, speed, vex::velocityUnits::rpm);
}

void moveBackwards(double feet, double speed){
    double degrees = feet * degreesPerFoot;
    RightFrontMotor.startRotateFor(-degrees, vex::rotationUnits::deg, -speed, vex::velocityUnits::rpm);
    RightRearMotor.startRotateFor(-degrees, vex::rotationUnits::deg, -speed, vex::velocityUnits::rpm);
    LeftFrontMotor.startRotateFor(-degrees, vex::rotationUnits::deg, -speed, vex::velocityUnits::rpm);
    LeftRearMotor.rotateFor(-degrees, vex::rotationUnits::deg, -speed, vex::velocityUnits::rpm);
}

// void turnLeft(double ang, double speed){
//     double degrees = (ang / 360) * degreesTo360;
//     LeftFrontMotor.startRotateTo(-degrees, vex::rotationUnits::deg, -speed, vex::velocityUnits::rpm);
//     RightFrontMotor.startRotateTo(degrees, vex::rotationUnits::deg, speed, vex::velocityUnits::rpm);
//     LeftRearMotor.startRotateTo(-degrees, vex::rotationUnits::deg, -speed, vex::velocityUnits::rpm);
//     RightRearMotor.rotateTo(degrees, vex::rotationUnits::deg, speed, vex::velocityUnits::rpm);
// }

// void turnRight(double ang, double speed){
//     double degrees = (ang / 360) * degreesTo360;
//     LeftFrontMotor.startRotateTo(degrees, vex::rotationUnits::deg, speed, vex::velocityUnits::rpm);
//     RightFrontMotor.startRotateTo(-degrees, vex::rotationUnits::deg, -speed, vex::velocityUnits::rpm);
//     LeftRearMotor.startRotateTo(degrees, vex::rotationUnits::deg, speed, vex::velocityUnits::rpm);
//     RightRearMotor.rotateTo(-degrees, vex::rotationUnits::deg, -speed, vex::velocityUnits::rpm);
// }

void drive_Tank(double deadzone){
  LeftSide.spin(vex::directionType::fwd, ((Controller1.Axis3.value() > deadzone || Controller1.Axis3.value() < -deadzone) ? Controller1.Axis3.value() : 0)/driveSpeedFactor, vex::velocityUnits::pct);
  RightSide.spin(vex::directionType::fwd, ((Controller1.Axis2.value() > deadzone || Controller1.Axis2.value() < -deadzone) ? Controller1.Axis2.value() : 0)/driveSpeedFactor, vex::velocityUnits::pct);
}

void drive_Arcade(double deadzone){
  LeftSide.spin(vex::directionType::fwd, (((Controller1.Axis3.value() > deadzone || Controller1.Axis3.value() < -deadzone) ? Controller1.Axis3.value() : 0) + ((Controller1.Axis1.value() > deadzone || Controller1.Axis1.value() < -deadzone) ? Controller1.Axis1.value() : 0))/driveSpeedFactor, vex::velocityUnits::pct);
  RightSide.spin(vex::directionType::fwd, (((Controller1.Axis3.value() > deadzone || Controller1.Axis3.value() < -deadzone) ? Controller1.Axis3.value() : 0) - ((Controller1.Axis1.value() > deadzone || Controller1.Axis1.value() < -deadzone) ? Controller1.Axis1.value() : 0))/driveSpeedFactor, vex::velocityUnits::pct);
}

void drive_Hybrid(double deadzone) {
  LeftSide.spin(vex::directionType::fwd, (((Controller1.Axis3.value() > deadzone || Controller1.Axis3.value() < -deadzone) ? Controller1.Axis3.value() : 0) - ((Controller1.Axis2.value() > deadzone || Controller1.Axis2.value() < -deadzone) ? Controller1.Axis2.value() : 0))/driveSpeedFactor, vex::velocityUnits::pct);
  RightSide.spin(vex::directionType::fwd, (((Controller1.Axis3.value() > deadzone || Controller1.Axis3.value() < -deadzone) ? Controller1.Axis3.value() : 0) + ((Controller1.Axis2.value() > deadzone || Controller1.Axis2.value() < -deadzone) ? Controller1.Axis2.value() : 0))/driveSpeedFactor, vex::velocityUnits::pct);
}

void spdToggle() {
  if (driveSpeedFactor == 1.25) {
    driveSpeedFactor = 2.5;
    driveSpeed = "Drive Speed Halved";
  } else if (driveSpeedFactor == 2.5) {
    driveSpeedFactor = 1.25;
    driveSpeed = "Drive Speed Normal";
  }
  wait(500, msec);
}

double vel(double distT, double distF) {
  double num = sin(M_PI * ((distT-distF)/distF)) * 127 + 10;
  return num;
}

void driveSelect() {
  if (mode == 0) {
    mode = 1;
    driveType = "Split Arcade Controls";
  } else if (mode == 1) {
    mode = 2;
    driveType = "Hybrid Controls";
  } else if (mode == 2) {
    mode = 0;
    driveType = "Tank Controls";
  }
}

void barMacro() {
  if (barPos == 0) {
    Intakes.spinFor(directionType::rev, 1, rotationUnits::rev);
    TilterMotor.spinFor(fwd, 250, deg, false);
    BarMotor.spinFor(directionType::fwd, 500, rotationUnits::deg);
    barPos = 1;
  } else if (barPos == 1) {
    BarMotor.spinFor(directionType::fwd, 250, rotationUnits::deg);
    barPos = 2;
  } else if (barPos == 2) {
    BarMotor.spinFor(directionType::rev, 750, deg, false);
    TilterMotor.spinFor(directionType::rev, 250, deg);
    barPos = 0;
  }
}

void fadeBack() {
  Intakes.spinFor(directionType::rev, 3, rotationUnits::rev, false);
  SmartDrive.driveFor(directionType::rev, 6, distanceUnits::in);
}

void autoRBS() {
  Drivetrain.driveFor(fwd, 8, inches);
  Drivetrain.driveFor(directionType::rev, 8, inches, false);
  flipOut();
  Intakes.setVelocity(100, pct);
  Drivetrain.setDriveVelocity(40, pct);
  Intakes.spinFor(fwd, 1.5, sec);
  Drivetrain.driveFor(fwd, 45.5, inches);
  // while (Drivetrain.isMoving()) {
  //   Intakes.spin(fwd, 100, pct);
  // }
  Drivetrain.setDriveVelocity(45, pct);
  // Drivetrain.driveFor(directionType::rev, 10.75, inches);
  // Intakes.stop();
  // Drivetrain.driveFor(directionType::rev, 24.75, inches);
  Drivetrain.driveFor(directionType::rev, 24.5, inches);
  Intakes.stop();
  // Drivetrain.turnFor(63, rotationUnits::deg);
  turnRight(128.55);
  Drivetrain.driveFor(fwd, 15.4, inches);
  Intakes.setVelocity(50, percentUnits::pct);
  Intakes.spinFor(directionType::rev, 0.725, rev);
  wait(100, msec);
  // deployStack();
  TilterMotor.setVelocity(35, pct);
  TilterMotor.spinFor(fwd, 555, deg);
  wait(50, msec);
  // TilterMotor.spinFor(directionType::rev, 10, deg);
  // Intakes.spinFor(directionType::rev, 0.25, rotationUnits::rev, 25, velocityUnits::pct);
  Drivetrain.driveFor(fwd, 0.6, inches);
  Drivetrain.driveFor(directionType::rev, 12, inches);
}

void autoBBS() {
  Drivetrain.driveFor(fwd, 8, inches);
  Drivetrain.driveFor(directionType::rev, 8, inches, false);
  flipOut();
  Intakes.setVelocity(100, pct);
  Drivetrain.setDriveVelocity(40, pct);
  Intakes.spinFor(fwd, 1.5, sec);
  Drivetrain.driveFor(fwd, 45.5, inches);
  // while (Drivetrain.isMoving()) {
  //   Intakes.spin(fwd, 100, pct);
  // }
  Drivetrain.setDriveVelocity(45, pct);
  // Drivetrain.driveFor(directionType::rev, 10.75, inches);
  // Intakes.stop();
  // Drivetrain.driveFor(directionType::rev, 24.75, inches);
  Drivetrain.driveFor(directionType::rev, 24.5, inches);
  Intakes.stop();
  // Drivetrain.turnFor(63, rotationUnits::deg);
  turnRight(128.575);
  Drivetrain.driveFor(fwd, 15.4, inches);
  Intakes.setVelocity(50, percentUnits::pct);
  Intakes.spinFor(directionType::rev, 0.725, rev);
  wait(100, msec);
  // deployStack();
  TilterMotor.setVelocity(35, pct);
  TilterMotor.spinFor(fwd, 555, deg);
  wait(50, msec);
  // TilterMotor.spinFor(directionType::rev, 10, deg);
  // Intakes.spinFor(directionType::rev, 0.25, rotationUnits::rev, 25, velocityUnits::pct);
  Drivetrain.driveFor(fwd, 0.6, inches);
  Drivetrain.driveFor(directionType::rev, 12, inches);
}

void autoRFS() {
  Drivetrain.driveFor(fwd, 10, inches);
  Drivetrain.driveFor(directionType::rev, 10, inches, false);
  flipOut();
  Drivetrain.driveFor(directionType::rev, 1, inches);
  Intakes.setVelocity(100, percentUnits::pct);
  Intakes.spinFor(0.75, sec);
  Drivetrain.driveFor(fwd, 24.4, inches);
  Drivetrain.driveFor(directionType::rev, 10.4, inches);
  turnLeft(90);
  Drivetrain.driveFor(fwd, 18.4, inches);
  turnLeft(125);
  Intakes.stop();
  Drivetrain.driveFor(fwd, 9, inches);
  Intakes.spinFor(directionType::rev, 0.55, rev);
  wait(100, msec);
  // deployStack();
  TilterMotor.setVelocity(35, pct);
  TilterMotor.spinFor(fwd, 500, deg);
  wait(100, msec);
  // Intakes.spinFor(directionType::rev, 2, rev, false);
  // Drivetrain.driveFor(directionType::rev, 12, inches);
  Drivetrain.driveFor(fwd, 0.6, inches);
  Drivetrain.driveFor(directionType::rev, 12, inches);
}

void autoBFS() {
  Drivetrain.driveFor(fwd, 10, inches);
  Drivetrain.driveFor(directionType::rev, 10, inches, false);
  flipOut();
  Drivetrain.driveFor(directionType::rev, 1, inches);
  Intakes.setVelocity(100, percentUnits::pct);
  Intakes.spinFor(0.75, sec);
  Drivetrain.driveFor(fwd, 24.4, inches);
  Drivetrain.driveFor(directionType::rev, 10.4, inches);
  turnRight(90);
  Drivetrain.driveFor(fwd, 18.4, inches);
  turnRight(125);
  Intakes.stop();
  Drivetrain.driveFor(fwd, 9, inches);
  Intakes.spinFor(directionType::rev, 0.55, rev);
  wait(100, msec);
  // deployStack();
  TilterMotor.setVelocity(35, pct);
  TilterMotor.spinFor(fwd, 500, deg);
  wait(100, msec);
  // Intakes.spinFor(directionType::rev, 2, rev, false);
  // Drivetrain.driveFor(directionType::rev, 12, inches);
  Drivetrain.driveFor(fwd, 0.6, inches);
  Drivetrain.driveFor(directionType::rev, 12, inches);
}

void autoSkills() {
  // Drivetrain.setDriveVelocity(40, pct);
  // Drivetrain.driveFor(directionType::rev, 16, inches);
  // wait(100, msec);
  // Drivetrain.driveFor(fwd, 22, inches); 
  // flipOut();
  // Drivetrain.driveFor(fwd, 12, inches);
  // Drivetrain.driveFor(directionType::rev, 12, inches, false);
  flipOut();
}

void autoExp() {
  Drivetrain.driveFor(fwd, 12, inches);
  Drivetrain.driveFor(directionType::rev, 12, inches, false);
  flipOut();
  Drivetrain.setDriveVelocity(40, pct);
  Drivetrain.driveFor(fwd, 36, inches, false);
  while (Drivetrain.isMoving()) {
    Intakes.spin(fwd, 100, pct);
  }
  turnRight(45);
  Drivetrain.driveFor(directionType::rev, 43.5, inches);
  turnLeft(0);
  Drivetrain.driveFor(fwd, 43.5, inches, false);
  while (Drivetrain.isMoving()) {
    Intakes.spin(fwd, 100, pct);
  }
  Drivetrain.driveFor(directionType::rev, 43.5, inches);
  turnLeft(114.5);
  Drivetrain.driveFor(fwd, 9, inches);
  Intakes.spinFor(directionType::rev, 0.625, rev);
  wait(100, msec);
  deployStack();
  wait(50, msec);
  // TilterMotor.spinFor(directionType::rev, 10, deg);
  Drivetrain.driveFor(fwd, 0.5, inches);
  Drivetrain.driveFor(directionType::rev, 12, inches);
}

void autonomous(void) {
  Drivetrain.setStopping(hold);
  Intakes.setStopping(coast);
  BarMotor.setStopping(brake);
  TilterMotor.setStopping(brake);
  Drivetrain.setDriveVelocity(50, pct);
  Inertial.resetRotation();
  if (auton == 0) {
    Controller1.Screen.clearScreen();
    Controller1.Screen.print("Running RFS");
    autoRFS();
  } else if (auton == 1) {
    Controller1.Screen.clearScreen();
    Controller1.Screen.print("Running BFS");
    autoBFS();
    } else if (auton == 2) {
    Controller1.Screen.clearScreen();
    Controller1.Screen.print("Running RBS");
    autoRBS();
  } else if (auton == 3) {
    Controller1.Screen.clearScreen();
    Controller1.Screen.print("Running BBS");
    autoBBS();
  } else if (auton == 4) {
    Controller1.Screen.clearScreen();
    Controller1.Screen.print("Running Skills");
    autoSkills();
  } else {
    Controller1.Screen.clearScreen();
    Controller1.Screen.print("No Auton Running");
  }
}

void back() {
  SmartDrive.driveFor(directionType::rev, 6, distanceUnits::in, false);
}

void usercontrol(void) {
  LeftSide.setStopping(coast);
  RightSide.setStopping(coast);
  Intakes.setStopping(coast);
  BarMotor.setStopping(hold);
  TilterMotor.setStopping(brake);
  BarMotor.resetRotation();
  Controller1.ButtonY.pressed(spdToggle);
  Controller1.ButtonX.pressed(driveSelect);
  Controller1.ButtonLeft.pressed(fadeBack);
  Controller1.ButtonB.pressed(back);
  while (1) {
    // Drive Commands
    if (mode == 0) {
      drive_Tank(17.5);
    } else if (mode == 1) {
      drive_Arcade(17.5);
    } else if (mode == 2) {
      drive_Hybrid(17.5);
    }
    
    // Tilter Commands
    if (Controller1.ButtonL1.pressing()) {
      if (Poten.angle(rotationUnits::raw) > 1750) {
        TilterMotor.spin(directionType::fwd, 25, velocityUnits::pct);
      } else {
        TilterMotor.spin(directionType::fwd, 35, velocityUnits::pct);
      }
    } else if (Controller1.ButtonL2.pressing()) {
      if (Poten.angle(rotationUnits::raw) > 515) {
        TilterMotor.spin(directionType::rev, 50, velocityUnits::pct);
      }
    } else {
      TilterMotor.stop(hold);
    }

    // Intake Commands
    if (Controller1.ButtonR1.pressing()) {
      Intakes.spin(directionType::fwd, intake, velocityUnits::pct);
    } else if (Controller1.ButtonR2.pressing()) {
      Intakes.spin(directionType::rev, 65, velocityUnits::pct);
    } else {
      Intakes.stop(brake);
    }

    // Bar Commands
    if (Controller1.ButtonUp.pressing()) {
      BarMotor.spin(directionType::fwd, allSpeed, velocityUnits::pct);
      if (Poten.angle(rotationUnits::raw) < 950) {
        TilterMotor.spin(directionType::fwd, 35.25, pct);
      }
    } else if (Controller1.ButtonDown.pressing()) {
      BarMotor.spin(directionType::rev, allSpeed, velocityUnits::pct);
      while (!(BarMotor.rotation(rotationUnits::deg) > 10)) {
        TilterMotor.spin(directionType::rev, 35.25, pct); 
        if (Poten.angle(rotationUnits::raw) < 550) {
          break;
        }
      }
      TilterMotor.stop();
    } else {
      BarMotor.stop(hold);
    }

    if (Controller1.ButtonA.pressing()) {
      deployStack();
    }

    if (Controller1.ButtonRight.pressing()) {
      flipOut();
      // barMacro();
    }

    Controller1.Screen.clearScreen();
    Controller1.Screen.setCursor(0, 0);
    Controller1.Screen.print(driveType.c_str());
    Controller1.Screen.newLine();
    Controller1.Screen.print(driveSpeed.c_str());

    // wait(20, msec);
  }
}

int main() {
  // Run the pre-autonomous function.
  pre_auton();

  // Set up callbacks for autonomous and driver control periods.
  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);

  // Prevent main from exiting with an infinite loop.
  while (true) {
    wait(100, msec);
  }
}