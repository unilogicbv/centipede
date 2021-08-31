/*
  Copyright (c) 2021, Amplified IT
  See the full description at http://labs.amplifiedit.com/centipede

  Support forums are available at https://currents.google.com/communities/100599537603662785064

  Published under an MIT License https://opensource.org/licenses/MIT
  
*/

#include <Keyboard.h>

/* Modify the following definations to fit your device version and whether you want Centipede to Powerwash the device on completion*/

#define device_version 91 // Change to the ChromeOS version you expect to use with Centipede; Changes have been reported in the following ranges 58-68, 69, 70
#define powerwash 1 //[0,1] Set to 0 after confirmation that script works through the enrollment process (Default 1).  
//Powerwash the device BEFORE enrollment is completed - NOTE: Will not work on ENROLLED devices. Used for Setting up Centipede.

/* Modify the following definitions to fit your wireless and enrollment credentials. */

#define wifi_name "---" // Define SSID for your wireless connection.
#define wifi_pass "---" // Define the password for your wireless connection.
#define wifi_security 2 //[0,1,2] Set to 0 for open, 1 for WEP, 2 for WPA
#define username "---" // Define the user name for enrolling the device.
#define password "---" // The password for the enrollment GAFE account.

/* Use these options to determine if you want to disable analytics, skip asset ID, or if you need to slow down the Centipede*/

#define sendUsageToGoogle 1 //[0,1] Set to 0 if you want to un-check the box to send usage analytics to Google
#define shutdown_when_finished 0 //[0,1] Set to 0 if you want Centipede to stop at the asset ID and location screen
#define selected_on_boot 3 //[0,1,2,3]  Active location when device poweron : 3 - "Let's go"(Default), 2 - accessibility, 1 - language, 0 - menu 

/* These are advanced options. The defaults should be fine, but feel free to tweak values below. */

#define setWiFi true //[true,false] Set to false for devices that already have WiFi setup and have accepted Terms of Service (ToS) 
#define enroll true //[true,false] Set to false for devices which only need WiFi setup

/* Use this area for advanced network setup options*/
#define advancedNetworkSetup false //[true,false] Set to true for EAP configuration, and fill out the definitions below
#define eapMethod "LEAP" // Valid options are "LEAP" "PEAP" "EAP-TLS" or "EAP-TTLS" - Note that they require the quotes to work properly
#define phaseTwoAuthentication 2 //[0,1,2,3,4,5,6] Set to 0 for automatic, 1 for EAP-MD5, 2 for MSCHAP(v2 pre v69; v1 V69+, 3 for MSCHAPv2, 4 for PAP, 5 for CHAP, 6 for GTC; v69+)
#define serverCaCertificateCheck 0 //[0,1] 0 is default, 1 is "Do not check"
#define subjectMatch "" // Fill in subject match here if needed for advanced wireless
#define identity "identity" // Fill in identity here if needed for advanced wireless
#define anonymousIdentity "" // Fill in anonymous identity here for advanced wireless
#define saveIdentityAndPassword 0 //[0,1] Set to 1 to save identity and password. NOT RECOMMENDED
#define sso 0 //[0,1] Set to 1 if using Single Sign On - NOTE: May need additional configuration in Advanced Network Setup around line 182.
#define retry 0 //attempt to retry enrollment in the event of an enrollment error;

/* Use this section for additional non-traditional methods*/
#define longer_enrollment_time 10 // Set to additional seconds to wait for Device Configuration and Enrollment
#define sign_in 0 //[0,1] Set to 1 to sign-in to the device after enrollment
#define remove_enrollment_wifi 0 //[0,1] Set to 1 to remove the enrollment wifi network. *sign_in also must be true* - NOTE: Only set to true when Chrome Device Network has been pulled down
#define enroll_device_cert 0 //[0,1] Set to 1 if enrolling device wide certificate *sign_in also must be true* - NOTE: Works best if user _*only*_ has Certificate Enrollment extension force installed

#define slowMode 1 // [0,1] Set to 1 if Centipede appears to be moving too quickly at any screen. This will slow down the entire process
#define update_wait_time 10 // Set to seconds to wait for mandatory updates to be downloaded

/* Do not modify anything below this line unless you're confident that you understand how to program Arduino or C */

// Version Definition
#define VERSION_69 (device_version >= 69)
#define VERSION_70 (device_version >= 70)
#define VERSION_78 (device_version >= 78)
#define VERSION_79 (device_version >= 79)
#define VERSION_83 (device_version >= 83)
#define VERSION_85 (device_version >= 85)
#define VERSION_86 (device_version >= 86)
#define VERSION_90 (device_version >= 90)
#define VERSION_91 (device_version >= 91)

// Special characters definition
#define KEY_LEFT_CTRL   0x80
#define KEY_LEFT_SHIFT  0x81
#define KEY_LEFT_ALT    0x82
#define KEY_RIGHT_CTRL  0x84
#define KEY_RIGHT_SHIFT 0x85
#define KEY_RIGHT_ALT   0x86
#define KEY_UP_ARROW    0xDA
#define KEY_DOWN_ARROW  0xD9
#define KEY_LEFT_ARROW  0xD8
#define KEY_RIGHT_ARROW 0xD7
#define KEY_BACKSPACE   0xB2
#define KEY_TAB         0xB3
#define KEY_ENTER       0xB0
#define KEY_ESC         0xB1
#define KEY_CAPS_LOCK   0xC1

int buttonPin = 2;  // Set a button to any pin
int RXLED = 17;
static uint8_t __clock_prescaler = (CLKPR & (_BV(CLKPS0) | _BV(CLKPS1) | _BV(CLKPS2) | _BV(CLKPS3)));

#define  LEFT_CTRL	1 << 0
#define  LEFT_SHIFT	1 << 1
#define  LEFT_ALT	1 << 2
#define  RIGHT_CTRL	1 << 3
#define  RIGHT_SHIFT	1 << 4
#define  RIGHT_ALT	1 << 5

class CKeyboard_ : public Keyboard_ {

  public:

  void blink() {
    digitalWrite(RXLED, LOW);
    //  TXLED1;
    delay(250);
    digitalWrite(RXLED, HIGH);
    //  TXLED0;
    delay(250);
  }

  void wait(int cycles) {
    for (int i = 0; i < cycles; i++) {
      blink();
      if (slowMode) {
        delay(250);
      }
    }
  }

  void writeRepeated(uint8_t c, int num) {
    for (int i = 0; i < num; i++) {
      Keyboard_::write(c);
      wait(1);
    }
  }

  void writeSpecial(uint8_t m, uint8_t c) {

    if (m & LEFT_CTRL)   { press(KEY_LEFT_CTRL);   };
    if (m & LEFT_SHIFT)  { press(KEY_LEFT_SHIFT);  };
    if (m & LEFT_ALT)    { press(KEY_LEFT_ALT);    };
    if (m & RIGHT_CTRL)  { press(KEY_RIGHT_CTRL);  };
    if (m & RIGHT_SHIFT) { press(KEY_RIGHT_SHIFT); };
    if (m & RIGHT_ALT)   { press(KEY_RIGHT_ALT);   };

    delay(50);
    Keyboard_::write(c);
    delay(50);

    if (m & LEFT_CTRL)   { release(KEY_LEFT_CTRL);   };
    if (m & LEFT_SHIFT)  { release(KEY_LEFT_SHIFT);  };
    if (m & LEFT_ALT)    { release(KEY_LEFT_ALT);    };
    if (m & RIGHT_CTRL)  { release(KEY_RIGHT_CTRL);  };
    if (m & RIGHT_SHIFT) { release(KEY_RIGHT_SHIFT); };
    if (m & RIGHT_ALT)   { release(KEY_RIGHT_ALT);   };

    wait(2);
  }
};

CKeyboard_ CKeyboard;

void setup()
{
  setPrescaler(); // Set prescaler to highest clock speed
  CKeyboard.begin(); // Start they keyboard emulator
  pinMode(buttonPin, INPUT);  // Set up the debugging pin. If you want to debug the code, use a length of wire to connect pins 2 and GND on the board
  digitalWrite(buttonPin, HIGH);

  pinMode(RXLED, OUTPUT); // Configure the on-board LED
  digitalWrite(RXLED, LOW);
  TXLED1;
  if (digitalRead(buttonPin) == 0) {
    showSuccess();
  }
  CKeyboard.wait(5); // Wait for all services to finish loading
}

void loop() { // Main Function - workflow is called within loop();
  if (digitalRead(buttonPin) == 1 ) { // Check for debugging. If not debugging, run the program
    showVersion();
    enterEnrollment();
    if (setWiFi) {
      wifiConfig(); // Enter the wifi configuration method (written down below)
      welcomeScreen();
      CKeyboard.wait(5);
      ToS(); // Accept Terms of Service
    }
      if (enroll){
      TXLED1; // Toggle the TX on-board LED
      CKeyboard.wait(15 + longer_enrollment_time); // Wait device to download configuration
      while (digitalRead(buttonPin) != 1) {
        bootLoop();
      }
      TXLED0;
  
      CKeyboard.wait(update_wait_time); // wait for device to update
      enterCredentials(); // Max progress with powerwash set to true - Will Powerwash after typing the password but before submitting
      CKeyboard.wait(50 + longer_enrollment_time); // wait for Enrollment to complete
      
      if(retry) {
          CKeyboard.write(KEY_ENTER); // press "Try again"
          CKeyboard.wait(2);
          enterCredentials(); // re-enter credentials
          CKeyboard.wait(2);
        }
        
      if (sign_in && shutdown_when_finished) { // Do not sign-in if "shutdown_when_finished" is false
        CKeyboard.write(KEY_ENTER);
        CKeyboard.wait(10);
        enterCredentials();
        CKeyboard.wait(90); // Wait for profile to load
        if (enroll_device_cert) {
          certificateEnrollment(); // Enroll Device wide Certificate
        }
        if (remove_enrollment_wifi) {
          removeEnrollmentWifi(); // Remove non-managed Enrollment WiFi
        }
      }
    }
    if (shutdown_when_finished) {
      shutDown();
    }
    showSuccess();
  }
  bootLoop();
}

void bootLoop() {
  //      digitalWrite(RXLED, LOW);   // set the LED on
  TXLED0; //TX LED is not tied to a normally controlled pin
  delay(200);              // wait for a second
  TXLED1;
  delay(200);
  TXLED0; //TX LED is not tied to a normally controlled pin
  delay(200);              // wait for a second
  TXLED1;
  delay(800);
}

void showSuccess() {
  digitalWrite(RXLED, HIGH);  // set the LED off
  while (true) {
    bootLoop();
  }
}

void enterCredentials() {
  CKeyboard.wait(5);
  CKeyboard.print(username);
  CKeyboard.wait(3);
  CKeyboard.write(KEY_ENTER);
  CKeyboard.wait(8);
  if (sso) {
    CKeyboard.write(KEY_TAB);
    CKeyboard.print(username);
    CKeyboard.write(KEY_TAB);
    CKeyboard.wait(2);
  }
  CKeyboard.print(password);
  if (powerwash) {
    CKeyboard.wait(5);
    Powerwash();
  }
  CKeyboard.wait(3);
  CKeyboard.write(KEY_ENTER);
  CKeyboard.wait(3);
}

void enterEnrollment() {
  CKeyboard.writeSpecial(LEFT_CTRL | LEFT_ALT, 'e');
}

void ToS() {
  // Terms of Service screen //Changed in v78
  int n;
  CKeyboard.wait(2);
  if (VERSION_79) {
    CKeyboard.press(KEY_LEFT_SHIFT);
    CKeyboard.wait(2);
  }
  switch(device_version) {
    case (78):
      n = 2;
      break;
    case (84):
      n = 3;
      break; 
    case 85:
      n = 6;
      break;
    case 86:
      CKeyboard.writeRepeated(KEY_TAB, 4); //Move into ToS iFrame
      CKeyboard.release(KEY_LEFT_SHIFT);
      n = 3;
      break;
    default:
      CKeyboard.writeRepeated(KEY_TAB, 1); //Move into ToS iFrame
      CKeyboard.release(KEY_LEFT_SHIFT);
      n = 3;
      break;
  }
  CKeyboard.wait(1);
  CKeyboard.writeRepeated(KEY_TAB, n);//Move to Send info to Google checkbox.
  CKeyboard.release(KEY_LEFT_SHIFT);
  CKeyboard.wait(2);
  if (!sendUsageToGoogle) {
    CKeyboard.write(KEY_ENTER);
    CKeyboard.wait(2);
  }
  CKeyboard.writeRepeated(KEY_TAB, 3 - VERSION_91); //Move to "Accept and continue"
  CKeyboard.wait(2);
  CKeyboard.write(KEY_ENTER);
}

void wifiConfig() {
  // Access the Network option from the system tray (Status Area).
  CKeyboard.writeSpecial(LEFT_ALT | LEFT_SHIFT ,'s');
  //to select the Network
  CKeyboard.writeRepeated(KEY_TAB, 3 + VERSION_70);  // 3 for pre v70, 4 for ver 70 (black menu)
  CKeyboard.wait(2);
  CKeyboard.write(KEY_ENTER);
  CKeyboard.wait(2);
  //to select the 'add Wifi' icon
  CKeyboard.writeRepeated(KEY_TAB, 3);
  CKeyboard.write(KEY_ENTER);
  CKeyboard.wait(2);
  // SSID
  CKeyboard.print(wifi_name);
  CKeyboard.wait(2);
  // TAB
  CKeyboard.write(KEY_TAB);
  CKeyboard.wait(2);
  if (wifi_security == 0) {
    CKeyboard.writeRepeated(KEY_TAB, 2);
  } else {
    if (advancedNetworkSetup) {
      setupAdvancedNetworkConfig();
    }
    else {
      CKeyboard.writeRepeated(KEY_DOWN_ARROW, wifi_security); //[1]WEP, [2]PSK (WPA or RSN), [3]EAP;

      // TAB
      CKeyboard.write(KEY_TAB); //[1,2]password, [3]EAP method;
      CKeyboard.wait(2);
      // type wifi password
      CKeyboard.print(wifi_pass);
      CKeyboard.writeRepeated(KEY_TAB, 3);
    }
  }
  CKeyboard.wait(2);
  // Enter
  CKeyboard.write(KEY_ENTER); // Connect
  // Delay 15 seconds to connect
  CKeyboard.wait(15);
}

void welcomeScreen() {
  CKeyboard.writeRepeated(KEY_TAB, 3 - selected_on_boot);
  CKeyboard.wait(2);
  CKeyboard.write(KEY_ENTER); // Click "Let's Go"
  CKeyboard.wait(2);
  if (!VERSION_78) {
    CKeyboard.writeRepeated(KEY_TAB, 2 + VERSION_70); // 3 for version 70+ (black menu)
    // After connecting, enter the enrollment key command to skip checking for update at this point in the process
  }
  enterEnrollment();
  CKeyboard.wait(2);
  CKeyboard.write(KEY_ENTER); // Click "Next"
}

void shutDown() { // Shutdown if not signed in, Sign out if signed in
  // Access the Network option from the system tray (Status Area).
  CKeyboard.writeSpecial(LEFT_ALT | LEFT_SHIFT ,'s');
  CKeyboard.writeRepeated(KEY_TAB, 1 + sign_in);
  CKeyboard.writeRepeated(KEY_ENTER, 1);
}

void setupAdvancedNetworkConfig() {
  //Starting at Security box
  if (VERSION_69 == 1) {
    CKeyboard.writeRepeated(KEY_DOWN_ARROW, 3); // Select Security "EAP" (v69);
    CKeyboard.write(KEY_TAB);
  } else {
    //ARROW_DOWN x3 WEP, PSK, EAP
    CKeyboard.writeRepeated(KEY_TAB, 2);
    CKeyboard.write(KEY_ENTER);
    CKeyboard.wait(2);
    //SSID (again);
    CKeyboard.print(wifi_name);
    CKeyboard.write(KEY_TAB);
    //@EAP Method
  }

  if (eapMethod == "LEAP") {
    // Default is LEAP v69+
    CKeyboard.writeRepeated(KEY_DOWN_ARROW, 1 - VERSION_69);
    CKeyboard.write(KEY_TAB);
    // Identity
    CKeyboard.print(identity);
    CKeyboard.write(KEY_TAB);
    CKeyboard.wait(2);
    CKeyboard.print(wifi_pass);
    CKeyboard.writeRepeated(KEY_TAB, 2);
    CKeyboard.wait(2);
    CKeyboard.write(KEY_ENTER); // Save Identity and Password (true);
    CKeyboard.writeRepeated(KEY_TAB, 2);
    CKeyboard.write(KEY_ENTER); // Connect;
  } else if (eapMethod == "PEAP") {
    // Select PEAP method
    CKeyboard.writeRepeated(KEY_DOWN_ARROW, 2 - VERSION_69);
    CKeyboard.write(KEY_TAB);
    CKeyboard.wait(2);
    // EAP Phase 2 authentication
    // If phase two authentication is defined, select it
    if (phaseTwoAuthentication) {
      CKeyboard.writeRepeated(KEY_DOWN_ARROW, phaseTwoAuthentication); // [0]Automatic, [1]EAP-MD5, [2]MSCHAP(v2 pre-v69;v1 v69+, [3]MSCHAPv2, [4]PAP, [5]CHAP, [6]GTC : v69)
    }
    CKeyboard.write(KEY_TAB);
    // Server CA Certificate
    if (serverCaCertificateCheck) {
      CKeyboard.write(KEY_DOWN_ARROW);//change to "DO NOT CHECK"
    }
    CKeyboard.write(KEY_TAB);

    // Identity
    CKeyboard.print(identity);
    CKeyboard.write(KEY_TAB);
    CKeyboard.wait(2);
    CKeyboard.print(wifi_pass);
    CKeyboard.writeRepeated(KEY_TAB, 2);

    // Anonymous Identity
    CKeyboard.print(anonymousIdentity);
    CKeyboard.write(KEY_TAB);
    CKeyboard.write(KEY_ENTER); //Save ID and PW
    CKeyboard.writeRepeated(KEY_TAB, 1 + VERSION_69); //End on Connect  /v69+
  } else if (eapMethod ==  "EAP-TLS") {
    // Select EAP-TLS method
    CKeyboard.writeRepeated(KEY_DOWN_ARROW, 2);
    CKeyboard.write(KEY_TAB);
    //EAP Phase 2 authentication
    // If phase two authentication is defined, select it
    if (phaseTwoAuthentication) {
      CKeyboard.writeRepeated(KEY_DOWN_ARROW, phaseTwoAuthentication); // [0]Automatic, [1]EAP-MD5, [2]MSCHAP(v2 pre-v69;v1 v69+, [3]MSCHAPv2, [4]PAP, [5]CHAP, [6]GTC : v69)
    }
    CKeyboard.write(KEY_TAB);
    // Server CA Certificate
    if (serverCaCertificateCheck) {
      CKeyboard.write(KEY_DOWN_ARROW); // Change to "DO NOT CHECK"
    }
    CKeyboard.write(KEY_TAB);

    // Subject match
    CKeyboard.print(subjectMatch);
    CKeyboard.write(KEY_TAB);

    // Identity
    CKeyboard.print(identity);
    CKeyboard.writeRepeated(KEY_TAB, 3);

  } else if (eapMethod == "EAP-TTLS") {
    CKeyboard.writeRepeated(KEY_DOWN_ARROW, 4);
    CKeyboard.write(KEY_TAB);

    // If phase two authentication is defined, select it
    if (phaseTwoAuthentication) {
      CKeyboard.writeRepeated(KEY_DOWN_ARROW, phaseTwoAuthentication);
    }
    CKeyboard.write(KEY_TAB);

    // Server CA Certificate
    if (serverCaCertificateCheck) {
      CKeyboard.write(KEY_DOWN_ARROW);
    }

    // Identity
    CKeyboard.print(identity);
    CKeyboard.write(KEY_TAB);
    CKeyboard.print(wifi_pass);
    CKeyboard.writeRepeated(KEY_TAB, 2);

    // Anonymous Identity
    CKeyboard.print(anonymousIdentity);
    CKeyboard.writeRepeated(KEY_TAB, 2);
  } else if (eapMethod ==  "EAP-TLS") {
    // Select EAP-TLS method
    CKeyboard.writeRepeated(KEY_DOWN_ARROW, 3);
    CKeyboard.write(KEY_TAB);

    // Server CA Certificate
    if (serverCaCertificateCheck) {
      CKeyboard.write(KEY_DOWN_ARROW);
    }
    CKeyboard.write(KEY_TAB);

    // Subject match
    CKeyboard.print(subjectMatch);
    CKeyboard.write(KEY_TAB);

    // Identity
    CKeyboard.print(identity);
    CKeyboard.writeRepeated(KEY_TAB, 3);

    // Anonymous Identity
    CKeyboard.print(anonymousIdentity);
    CKeyboard.writeRepeated(KEY_TAB, 2);
  }
}

void reloadPolicies() {
  CKeyboard.wait(3);
  newTab();
  CKeyboard.print("chrome://policy");
  CKeyboard.write(KEY_ENTER);
  CKeyboard.wait(2);
  CKeyboard.writeRepeated(KEY_TAB, 1);
  CKeyboard.wait(2);
  CKeyboard.write(KEY_ENTER);
  CKeyboard.wait(2);
  closeTab();
}

void removeEnrollmentWifi() {
  CKeyboard.wait(7);
  reloadPolicies();
  newTab();
  CKeyboard.print("chrome://settings/knownNetworks?type=WiFi");
  CKeyboard.write(KEY_ENTER);
  CKeyboard.wait(5);
  CKeyboard.writeRepeated(KEY_TAB, 3); // Select the Top Network's "More options"
  CKeyboard.wait(2);
  CKeyboard.write(KEY_ENTER);
  CKeyboard.wait(2);
  CKeyboard.writeRepeated(KEY_DOWN_ARROW, 3); // Select "Forget";
  CKeyboard.wait(2);
  CKeyboard.write(KEY_ENTER);
  CKeyboard.wait(5);
  closeTab();
}


void newTab() {
  CKeyboard.writeSpecial(RIGHT_CTRL, 'n');
}
void closeTab() {
  CKeyboard.writeSpecial(RIGHT_CTRL, 'w');
}
void showVersion() {
if (VERSION_90){
  /*Start ChromeVOX escape*/
    CKeyboard.write(KEY_ESC);
    CKeyboard.wait(1);
    CKeyboard.write(KEY_TAB);
    CKeyboard.writeSpecial(RIGHT_SHIFT, KEY_TAB);
  /*End ChromeVOX Escape*/
  }
  CKeyboard.writeSpecial(RIGHT_ALT, 'v');
}
void Powerwash() {
  CKeyboard.writeSpecial(RIGHT_CTRL | RIGHT_ALT | RIGHT_SHIFT, 'r');
  if (VERSION_78) {
    CKeyboard.write(KEY_TAB);
    CKeyboard.wait(2);
  }
  if (VERSION_86){
    CKeyboard.write(KEY_TAB);
    CKeyboard.wait(2);
  }
  CKeyboard.write(KEY_ENTER);
  CKeyboard.wait(2);
  CKeyboard.write(KEY_TAB);
  CKeyboard.write(KEY_ENTER);
}
void certificateEnrollment() {
  CKeyboard.wait(5);
  CKeyboard.writeRepeated(KEY_TAB, 2);
  CKeyboard.print(username); //Enter Username for Certificate Enrollment
  CKeyboard.write(KEY_TAB);
  CKeyboard.wait(2);
  CKeyboard.print(password); //Enter Password for Certificate Enrollment
  CKeyboard.write(KEY_TAB);
  CKeyboard.wait(2);
  CKeyboard.write(KEY_ENTER); //Enable Device Wide certificate for Certificate Enrollment
  CKeyboard.wait(2);
  CKeyboard.writeRepeated(KEY_TAB, 4);
  CKeyboard.wait(2);
  CKeyboard.write(KEY_ENTER);
  CKeyboard.wait(40);
}
void setPrescaler() {
#if defined(ARDUINO_ARCH_AVR)
  // Disable interrupts.
  uint8_t oldSREG = SREG;
  cli();

  // Enable change.
  CLKPR = _BV(CLKPCE); // write the CLKPCE bit to one and all the other to zero

  // Change clock division.
  CLKPR = 0x0; // write the CLKPS0..3 bits while writing the CLKPE bit to zero

  // Copy for fast access.
  __clock_prescaler = 0x0;

  // Recopy interrupt register.
  SREG = oldSREG;
#endif // ARDUINO_ARCH_AVR
}

//TODO:
#define in_developer_mode 0 // Set to 1 if device is in developer mode 
