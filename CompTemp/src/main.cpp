#define _USE_MATH_DEFINES

#include "vex.h"
#include <cmath>

using namespace vex;
int driveSpeedFactor = 1;
int auton = -1;
int allSpeed = 100;
int intake = 100;
int mode = 0;
std::string driveType = "Tank Controls";
std::string driveSpeed = "Drive Speed Normal"; 


// A global instance of competition
competition Competition;
motor RightFrontMotor = motor(PORT13, ratio18_1, true);
motor RightRearMotor = motor(PORT12, ratio18_1, true);
motor LeftRearMotor = motor(PORT19, ratio18_1, false);
motor LeftFrontMotor = motor(PORT18, ratio18_1, false);
motor LeftIntakeMotor = motor(PORT20, ratio18_1, false);
motor RightIntakeMotor = motor(PORT11, ratio18_1, true);
motor TilterMotor = motor(PORT17, ratio36_1, false);
motor BarMotor = motor(PORT14, ratio36_1, true);
//pot Poten = pot(Brain.ThreeWirePort.A);
motor_group LeftSide = motor_group(LeftFrontMotor, LeftRearMotor);
motor_group RightSide = motor_group(RightFrontMotor, RightRearMotor);
motor_group Intakes = motor_group(LeftIntakeMotor, RightIntakeMotor);
drivetrain Drivetrain = drivetrain(LeftSide, RightSide, 319.19, 355.59999999999997, 292.09999999999997, mm, 1.0);
controller Controller1;

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
}

/*void deployStack() {
  TilterMotor.setBrake(brake);
  int error = 2000;
  int tilt_speed = 100;
  while(error > 100 && tilt_speed > 5) {
    error = 3500-Poten.value(rotationUnits::raw);
    tilt_speed = error * 100/1700;
    TilterMotor.spin(fwd, tilt_speed, pct);
  }
}*/

void flipOut() {
  TilterMotor.setVelocity(100, pct);
  BarMotor.setVelocity(100, pct);
  TilterMotor.spinFor(fwd, 425, deg);
  wait(50, msec);
  TilterMotor.spinFor(directionType::rev, 425, deg, false);
  BarMotor.spinFor(fwd, 400, deg);
  wait(1, msec);
  BarMotor.spinFor(directionType::rev, 400, deg);
}

void autoRFS() {
  // Drivetrain.driveFor(fwd, 6, inches);
  // Drivetrain.driveFor(directionType::rev, 6, inches, false);
  // flipOut();
  Drivetrain.driveFor(fwd, 42.5, inches, false);
  while (Drivetrain.isMoving()) {
    Intakes.spin(fwd, 100, pct);
  }
  Drivetrain.driveFor(directionType::rev, 43, inches);
  Drivetrain.driveFor(fwd, 8, inches);
  Drivetrain.turnFor(80, deg);
  Intakes.stop();
  Drivetrain.driveFor(fwd, 10, inches);
  Intakes.spinFor(directionType::rev, 2, rev);
  TilterMotor.setVelocity(40, pct);
  wait(100, msec);
  TilterMotor.spinFor(fwd, 475, deg);  Intakes.spinFor(directionType::rev, 2, rev, false);
  Drivetrain.driveFor(directionType::rev, 12, inches);
}

void autoBFS() {
  Drivetrain.driveFor(fwd, 6, inches);
  Drivetrain.driveFor(directionType::rev, 6, inches, false);
  flipOut();
  Drivetrain.driveFor(fwd, 42.5, inches, false);
  while (Drivetrain.isMoving()) {
    Intakes.spin(fwd, 100, pct);
  }
  Drivetrain.driveFor(directionType::rev, 43, inches);
  Drivetrain.driveFor(fwd, 6, inches);
  Drivetrain.turnFor(-80, deg);
  Intakes.stop();
  Drivetrain.driveFor(fwd, 10, inches);
  Intakes.spinFor(directionType::rev, 2, rev);
  TilterMotor.setVelocity(40, pct);
  wait(100, msec);
  TilterMotor.spinFor(fwd, 475, deg);
  wait(100, msec);
  Intakes.spinFor(directionType::rev, 2, rev, false);
  Drivetrain.driveFor(directionType::rev, 12, inches);
}

void autoRBS() {
  Drivetrain.setDriveVelocity(50, pct);
  Drivetrain.driveFor(directionType::rev, 16, inches);
  wait(100, msec);
  Drivetrain.driveFor(fwd, 22, inches);
  flipOut();
}

void autoBBS() {
  Drivetrain.setDriveVelocity(50, pct);
  Drivetrain.driveFor(directionType::rev, 16, inches);
  wait(100, msec);
  Drivetrain.driveFor(fwd, 22, inches);
  flipOut();
}

void autoSkills() {
  Drivetrain.setDriveVelocity(50, pct);
  Drivetrain.driveFor(directionType::rev, 16, inches);
  wait(100, msec);
  Drivetrain.driveFor(fwd, 22, inches);
}

void autonomous(void) {
  Drivetrain.setStopping(hold);
  Intakes.setStopping(coast);
  BarMotor.setStopping(brake);
  TilterMotor.setStopping(brake);
  Drivetrain.setDriveVelocity(25, pct);
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
    // Doesn't run Auton
    Controller1.Screen.clearScreen();
    Controller1.Screen.print("No Auton Running");
  }
}

double vel(double distT, double distF) {
  double num = sin(M_PI * ((distT-distF)/distF)) * 127 + 10;
  return num;
}

void intToggle() {
  while (intake) {
    Intakes.spin(fwd, 100, pct);
    if (Controller1.ButtonRight.pressing()) {
      break;
    }
  }
}

void usercontrol(void) {
  LeftSide.setStopping(coast);
  RightSide.setStopping(coast);
  Intakes.setStopping(coast);
  BarMotor.setStopping(hold);
  TilterMotor.setStopping(brake);
  while (1) {
    // Drive Commands
    if (mode == 0) {
      LeftSide.spin(directionType::fwd, ((Controller1.Axis3.value()*std::abs(Controller1.Axis3.value())/100)/driveSpeedFactor), velocityUnits::pct);
      RightSide.spin(directionType::fwd, ((Controller1.Axis2.value()*std::abs(Controller1.Axis2.value())/100)/driveSpeedFactor), velocityUnits::pct);
    } else if (mode == 1) {
      LeftSide.spin(directionType::fwd, ((Controller1.Axis3.value()+Controller1.Axis1.value())/2)/driveSpeedFactor, velocityUnits::pct);
      RightSide.spin(directionType::fwd, ((Controller1.Axis3.value()-Controller1.Axis1.value())/2)/driveSpeedFactor, velocityUnits::pct);
    }
    
    // Tilter Commands
    if (Controller1.ButtonL1.pressing()) {
      TilterMotor.spin(directionType::fwd, 35, velocityUnits::pct);
    } else if (Controller1.ButtonL2.pressing()) {
      TilterMotor.spin(directionType::rev, 35, velocityUnits::pct);
    } else {
      TilterMotor.stop(brake);
    }

    // Intake Commands
    if (Controller1.ButtonR1.pressing()) {
      Intakes.spin(directionType::fwd, intake, velocityUnits::pct);
    } else if (Controller1.ButtonR2.pressing()) {
      Intakes.spin(directionType::rev, 50, velocityUnits::pct);
    } else {
      Intakes.stop(coast);
    }

    // Bar Commands
    if (Controller1.ButtonUp.pressing()) {
      BarMotor.spin(directionType::fwd, allSpeed, velocityUnits::pct);
      TilterMotor.spin(directionType::fwd, 32, pct);
    } else if (Controller1.ButtonDown.pressing()) {
      BarMotor.spin(directionType::rev, allSpeed, velocityUnits::pct);
      TilterMotor.spin(directionType::rev, 32, pct);
    } else {
      BarMotor.stop(hold);
    }

    // Speed Control
    // if (Controller1.ButtonA.pressing()) {
    //   if (driveSpeedFactor == 1) {
    //   driveSpeedFactor = 2;

    //   driveSpeed = "Drive Speed Halved";
    //   }
    //   else if (driveSpeedFactor == 2) {
    //   driveSpeedFactor = 1;
    //   driveSpeed = "Drive Speed Normal";
    //   }
    //   wait(50, msec);
    // }

    // Drive Select
    if (Controller1.ButtonX.pressing()) {
      if (mode == 0) {
        mode = 1;
        driveType = "Split Arcade Controls";
      } else if (mode == 1) {
        mode = 0;
        driveType = "Tank Controls";
      }
      wait(50, msec);
    }

    if (Controller1.ButtonLeft.pressing()) {
      flipOut();
    }

    Controller1.Screen.clearScreen();
    Controller1.Screen.setCursor(0, 0);
    Controller1.Screen.print(driveType.c_str());
    Controller1.Screen.newLine();
    // Controller1.Screen.print(driveSpeed.c_str());

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