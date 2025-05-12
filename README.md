# IKT104 Exam Project

## How to Run

1. **Open the project in Mbed Studio.**
2. **Set up the Mbed library:**
   - Either create a **symlink** to an existing Mbed library instance **or** copy the Mbed library directly into the project folder.
   - Ensure **Mbed OS version 6** is checked out.
3. **Build the project:**
   - Select your target hardware.
   - Confirm that the correct **ports for all components** are set.

---

## Component Configuration

| Component                | Pin(s)      | Notes             |
|--------------------------|-------------|-------------------|
| **Button - Left**        | D0          | Internal pull-up  |
| **Button - Up**          | D2          | Internal pull-up  |
| **Button - Down**        | D3          | Internal pull-up  |
| **Button - Right**       | D4          | Internal pull-up  |
| **Piezo Buzzer**         | D13         |                   |
| **Display (RGB LCD 1602)** | SDA: D14, SCL: D15 | I²C connection   |
| **Sensor**               | -           | Mounted to board  |
## Breadboard Configuration

![Breadboard Setup](Mikrokontroller%20Layout/20250511_223532.jpg)
