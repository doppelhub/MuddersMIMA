//Copyright 2022-2024(c) John Sullivan

#include "muddersMIMA.h"

//==============================================================================
// Simplified Single Data Stream - LiControl Slave
// Clean implementation with no old protocol remnants
//==============================================================================

#define SPI_PACKET_SIZE     7

// Packet headers
#define LIBCM_PACKET_HEADER      0x55  // LiBCM sends this
#define LICONTROL_PACKET_HEADER  0xAA  // LiControl responds with this

//==============================================================================
// Global Variables
//==============================================================================

static volatile bool spi_packetReceived = false;
static volatile uint8_t spi_rxBuffer[SPI_PACKET_SIZE];
static volatile uint8_t spi_txBuffer[SPI_PACKET_SIZE];
static volatile uint8_t spi_byteIndex = 0;

// Data from LiBCM
static int16_t libcm_motor_power_watts = 0;
static uint8_t libcm_ok_to_start = 0;
static uint8_t libcm_battery_soc = 0;
static uint8_t libcm_battery_temp = 0;
static uint8_t libcm_mode_override = 0xFF; // 0xFF = no override, 0-2 = force specific mode

// Data to send to LiBCM
static uint8_t licontrol_heartbeat = 0;
static uint16_t licontrol_motor_rpm = 0;
static uint8_t licontrol_vehicle_speed = 0;
static uint8_t licontrol_mode = 0;         // Current toggle switch position (0-2)
static uint8_t licontrol_status_flags = 1; // bit 0 = system OK

//==============================================================================
// Helper Functions
//==============================================================================

uint8_t calculateChecksum(volatile uint8_t* packet) {
    uint8_t sum = 0;
    for(int i = 0; i < SPI_PACKET_SIZE - 1; i++) {
        sum += packet[i];
    }
    return sum;
}

void buildResponsePacket(volatile uint8_t* packet) {
    // Update our data from MIMA systems
    licontrol_heartbeat++;                             // Increment heartbeat
    licontrol_motor_rpm = engineSignals_getLatestRPM(); // Get actual RPM
    licontrol_vehicle_speed = engineSignals_getLatestVehicleMPH(); // Get speed
    licontrol_mode = gpio_getButton_toggle();          // Get current toggle switch position
    licontrol_status_flags = 1; // System OK
    
    // Build response packet
    packet[0] = LICONTROL_PACKET_HEADER;            // 0xAA
    packet[1] = licontrol_heartbeat;                // Heartbeat counter
    packet[2] = (licontrol_motor_rpm >> 8) & 0xFF; // RPM high byte
    packet[3] = licontrol_motor_rpm & 0xFF;         // RPM low byte
    packet[4] = licontrol_vehicle_speed;            // Speed in mph
    packet[5] = licontrol_mode;                     // Toggle switch position (0-2)
    packet[6] = calculateChecksum(packet);          // Checksum
}

//==============================================================================
// SPI ISR
//==============================================================================

ISR(SPI_STC_vect)
{
	uint8_t receivedByte = SPDR;
	
	if (spi_byteIndex < SPI_PACKET_SIZE) {
		spi_rxBuffer[spi_byteIndex] = receivedByte;
		spi_byteIndex++;
		
		if (spi_byteIndex < SPI_PACKET_SIZE) {
			SPDR = spi_txBuffer[spi_byteIndex];
		} else {
			spi_packetReceived = true;
		}
	}
}

//==============================================================================
// Public Interface Functions
//==============================================================================

void spiToLiBCM_begin(void)
{
	// Configure SPI pins for slave mode
	pinMode(MISO, OUTPUT);
	pinMode(MOSI, INPUT);
	pinMode(SCK, INPUT);
	pinMode(SS, INPUT);
	
	digitalWrite(SS, HIGH); // Pull-up on SS
	
	// Enable SPI slave with interrupts
	SPCR = (1 << SPE) | (1 << SPIE);
	
	// Clear registers
	uint8_t temp = SPSR;
	temp = SPDR;
	(void)temp;
	
	// Initialize variables
	spi_byteIndex = 0;
	spi_packetReceived = false;
	
	// Build initial response packet
	buildResponsePacket(spi_txBuffer);
	
	// Load first response byte
	SPDR = spi_txBuffer[0];
	
	Serial.println(F("LiControl slave initialized - Simplified data stream"));
	Serial.println(F("Receives: Power(W), OK-to-start, SoC(%), Temp(C+40)"));
	Serial.println(F("Sends: Heartbeat, RPM, Speed(mph), Mode(0-9)"));
}

void spiToLiBCM_handler(void)
{
	// CS edge detection for transaction reset
	static uint8_t last_ss = HIGH;
	uint8_t current_ss = digitalRead(SS);
	
	if (last_ss == HIGH && current_ss == LOW) {
		// New transaction starting
		cli();
		spi_byteIndex = 0;
		spi_packetReceived = false;
		
		// Build fresh response
		buildResponsePacket(spi_txBuffer);
		SPDR = spi_txBuffer[0];
		sei();
	}
	last_ss = current_ss;
	
	// Process received packets
	if (spi_packetReceived) {
		spi_packetReceived = false;
		
		// Verify and process LiBCM data
		if (spi_rxBuffer[0] == LIBCM_PACKET_HEADER) {
			uint8_t calc_checksum = calculateChecksum(spi_rxBuffer);
			if (calc_checksum == spi_rxBuffer[6]) {
				// Valid packet - extract data
				libcm_motor_power_watts = (spi_rxBuffer[1] << 8) | spi_rxBuffer[2];
				libcm_ok_to_start = spi_rxBuffer[3];
				libcm_battery_soc = spi_rxBuffer[4];
				libcm_battery_temp = spi_rxBuffer[5];
				// Note: spi_rxBuffer[6] is checksum
				
				Serial.print(F("LiBCM: Pwr="));
				Serial.print(libcm_motor_power_watts);
				Serial.print(F("W OK="));
				Serial.print(libcm_ok_to_start);
				Serial.print(F(" SoC="));
				Serial.print(libcm_battery_soc);
				Serial.print(F("% Temp="));
				Serial.print(libcm_battery_temp - 40);
				Serial.println(F("C"));
			}
		}
		
		// Reset for next transaction
		spi_byteIndex = 0;
		buildResponsePacket(spi_txBuffer);
		SPDR = spi_txBuffer[0];
	}
}

//==============================================================================
// Data Access Functions (for MIMA system to read LiBCM data)
//==============================================================================

bool spiToLiBCM_isConnected(void)
{
	// Simple connection check - could be enhanced with timeout
	return true;
}

int16_t spiToLiBCM_getLiBCM_Power_watts(void)
{
	return libcm_motor_power_watts;
}

uint8_t spiToLiBCM_getLiBCM_OkToStart(void)
{
	return libcm_ok_to_start;
}

uint8_t spiToLiBCM_getLiBCM_SoC_percent(void)
{
	return libcm_battery_soc;
}

uint8_t spiToLiBCM_getLiBCM_Temperature_celsius(void)
{
	return libcm_battery_temp - 40; // Remove the +40 offset
}

uint8_t spiToLiBCM_getLiBCM_ModeOverride(void)
{
	return libcm_mode_override; // 0xFF = no override, 0-9 = forced mode
}

// Function to get the effective operating mode (considering override)
uint8_t spiToLiBCM_getEffectiveMode(void)
{
	uint8_t switch_position = gpio_getButton_toggle();
	
	// If switch is in position 0 AND LiBCM has set an override, use override
	if(switch_position == 0 && libcm_mode_override != 0xFF && libcm_mode_override <= 9) {
		return libcm_mode_override;
	}
	
	// Otherwise use the physical switch position
	return switch_position;
}