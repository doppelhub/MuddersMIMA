#include "muddersMIMA.h"

/////////////////////////////////////////////////////////////////////////////////////////////

// New function to handle clutch lockout with ramping and hysteresis
uint8_t mcm_applyClutchLockout(uint8_t requestedCMDPWR_percent, uint8_t mamode1_state)
{
    #ifdef ENABLE_CLUTCH_LOCKOUT
    
    // Apply lockout to manual joystick modes (assist, regen, and idle)
    if (mamode1_state != MAMODE1_STATE_IS_ASSIST && 
        mamode1_state != MAMODE1_STATE_IS_REGEN && 
        mamode1_state != MAMODE1_STATE_IS_IDLE) {
        return requestedCMDPWR_percent;
    }
    
    static bool clutchLockoutActive = false;
    static uint32_t clutchReleaseTime_ms = 0;
    static uint32_t lastClutchPressTime_ms = 0;
    static bool lastClutchState = CLUTCH_PEDAL_RELEASED;
    static uint32_t lastDebugTime_ms = 0;
    
    bool currentClutchState = gpio_getClutchPosition();
    uint32_t currentTime_ms = millis();
    
    // Detect clutch state changes with hysteresis
    if (currentClutchState != lastClutchState) {
        // Only process state change if enough time has passed (hysteresis)
        if ((currentTime_ms - lastClutchPressTime_ms) > CLUTCH_LOCKOUT_HYSTERESIS_MS) {
            
            if (currentClutchState == CLUTCH_PEDAL_PRESSED) {
                // Clutch just pressed - activate lockout immediately
                clutchLockoutActive = true;
                lastClutchPressTime_ms = currentTime_ms;
                Serial.println(F("CLUTCH: Lockout ACTIVATED"));
            }
            else {
                // Clutch just released - start timer
                clutchReleaseTime_ms = currentTime_ms;
                lastClutchPressTime_ms = currentTime_ms;
                Serial.println(F("CLUTCH: Released, starting timer"));
            }
            
            lastClutchState = currentClutchState;
        }
    }
    
    // If clutch is currently pressed, full lockout
    if (currentClutchState == CLUTCH_PEDAL_PRESSED) {
        clutchLockoutActive = true;
        return 50; // Return neutral position (50%)
    }
    
    // If clutch was recently released, check timer
    if (clutchLockoutActive && currentClutchState == CLUTCH_PEDAL_RELEASED) {
        uint32_t timeSinceRelease_ms = currentTime_ms - clutchReleaseTime_ms;
        
        if (timeSinceRelease_ms < CLUTCH_LOCKOUT_DELAY_MS) {
            // Still in lockout period
            return 50; // Return neutral position
        }
        else if (timeSinceRelease_ms < (CLUTCH_LOCKOUT_DELAY_MS + CLUTCH_LOCKOUT_RAMP_TIME_MS)) {
            // In ramp-up period
            uint32_t rampTime = timeSinceRelease_ms - CLUTCH_LOCKOUT_DELAY_MS;
            uint8_t rampPercent = (rampTime * 100) / CLUTCH_LOCKOUT_RAMP_TIME_MS;
            
            // Blend from neutral (50%) to requested value
            if (requestedCMDPWR_percent >= 50) {
                // Assist case: ramp from 50% to requested
                uint16_t assistAmount = requestedCMDPWR_percent - 50;
                uint8_t rampedCMDPWR = 50 + ((assistAmount * rampPercent) / 100);
                return rampedCMDPWR;
            } else {
                // Regen case: ramp from 50% to requested
                uint16_t regenAmount = 50 - requestedCMDPWR_percent;
                uint8_t rampedCMDPWR = 50 - ((regenAmount * rampPercent) / 100);
                return rampedCMDPWR;
            }
        }
        else {
            // Ramp period complete - disable lockout
            clutchLockoutActive = false;
            Serial.println(F("CLUTCH: Lockout DEACTIVATED"));
        }
    }
    
    #endif // ENABLE_CLUTCH_LOCKOUT
    
    return requestedCMDPWR_percent;
}

/////////////////////////////////////////////////////////////////////////////////////////////

// RPM-based power limiting function
uint8_t mcm_applyRPMPowerLimiting(uint8_t requestedCMDPWR_percent, uint8_t mamode1_state)
{
    // Only apply RPM power limiting to assist mode
    if (mamode1_state != MAMODE1_STATE_IS_ASSIST) {
        return requestedCMDPWR_percent;
    }
    
    uint16_t currentRPM = engineSignals_getLatestRPM();
    uint8_t limitedCMDPWR = requestedCMDPWR_percent;
    
    // Static variables for smooth control
    static bool lowRpmLimitActive = false;
    static uint32_t lastTransitionTime_ms = 0;
    static uint8_t lastOutputCMDPWR = 50; // Remember last output for smooth transitions
    static uint32_t lastControlUpdate_ms = 0;
    static bool controlInitialized = false; // Track if we've initialized the control
    
    // Calculate RPM thresholds with hysteresis
    uint16_t rpmLimitStart = RPM_THRESHOLD_FOR_POWER_LIMIT;
    uint16_t rpmLimitEnd = RPM_THRESHOLD_FOR_POWER_LIMIT + RPM_POWER_LIMIT_RAMP_WINDOW;
    uint16_t rpmHysteresis = RPM_POWER_LIMIT_HYSTERESIS;
    
    // Prevent rapid transitions (minimum 500ms between state changes)
    uint32_t currentTime_ms = millis();
    bool transitionAllowed = (currentTime_ms - lastTransitionTime_ms) > 500;
    
    // Determine if we should apply low RPM limiting with hysteresis
    if (!lowRpmLimitActive) {
        // Currently not limiting - check if we should start
        if (currentRPM < (rpmLimitStart - rpmHysteresis) && transitionAllowed) {
            lowRpmLimitActive = true;
            lastTransitionTime_ms = currentTime_ms;
            controlInitialized = false; // Reset control initialization
        }
    } else {
        // Currently limiting - check if we should stop
        if (currentRPM > (rpmLimitEnd + rpmHysteresis) && transitionAllowed) {
            lowRpmLimitActive = false;
            lastTransitionTime_ms = currentTime_ms;
            controlInitialized = false; // Reset control initialization
        }
    }
    
    // Apply power control in the low RPM zone
    if (lowRpmLimitActive) {
        // Check if LiBCM connection is available
        bool libcm_connected = spiToLiBCM_isConnected();
        
        if (libcm_connected) {
            // Initialize control with safe default on first entry
            if (!controlInitialized) {
                // Start with conservative 60% ceiling
                uint8_t safeStartingPoint = 60;
                if (requestedCMDPWR_percent > JOYSTICK_NEUTRAL_MAX_PERCENT) {
                    lastOutputCMDPWR = map(requestedCMDPWR_percent, 
                                          JOYSTICK_NEUTRAL_MAX_PERCENT, 100, 
                                          JOYSTICK_NEUTRAL_MAX_PERCENT, safeStartingPoint);
                } else {
                    lastOutputCMDPWR = requestedCMDPWR_percent;
                }
                controlInitialized = true;
            }
            
            // LiBCM connected - use gentle power monitoring
            int16_t actualPower_watts = spiToLiBCM_getLiBCM_Power_watts();
            
            // Only apply control if we're outputting positive power (assist mode)
            if (actualPower_watts > 0) {
                // Calculate how much we're over the limit (strict 13kW)
                int16_t powerError = actualPower_watts - MAX_ASSIST_POWER_WATTS_LOW_RPM;
                
                // Apply gentle control if we're exceeding the limit
                if (powerError > 500) { // 500W buffer to prevent oscillation
                    // Update control slowly to prevent IMA faults (every 200ms)
                    if ((currentTime_ms - lastControlUpdate_ms) >= 200) {
                        lastControlUpdate_ms = currentTime_ms;
                        
                        // Reduce by 1-3% at a time
                        uint8_t reduction = 1;
                        if (powerError > 2000) reduction = 2; // More aggressive if way over
                        if (powerError > 4000) reduction = 3; // Even more if extremely over
                        
                        // Apply gentle reduction
                        if (lastOutputCMDPWR > (10 + reduction)) {
                            lastOutputCMDPWR -= reduction;
                        }
                        
                        limitedCMDPWR = lastOutputCMDPWR;
                    } else {
                        // Not time to update - use previous value for stability
                        limitedCMDPWR = lastOutputCMDPWR;
                    }
                } else if (powerError < -1000) {
                    // Power is well under limit - allow gentle increase
                    if ((currentTime_ms - lastControlUpdate_ms) >= 200) {
                        lastControlUpdate_ms = currentTime_ms;
                        
                        // Gentle increase toward requested value, but don't exceed safe ceiling
                        uint8_t safeCeiling = 70; // Conservative ceiling even when under limit
                        uint8_t targetCMDPWR = requestedCMDPWR_percent;
                        
                        if (requestedCMDPWR_percent > JOYSTICK_NEUTRAL_MAX_PERCENT) {
                            targetCMDPWR = map(requestedCMDPWR_percent, 
                                              JOYSTICK_NEUTRAL_MAX_PERCENT, 100, 
                                              JOYSTICK_NEUTRAL_MAX_PERCENT, safeCeiling);
                        }
                        
                        if (lastOutputCMDPWR < targetCMDPWR && lastOutputCMDPWR < 95) {
                            lastOutputCMDPWR += 1; // Very gentle increase
                        }
                        
                        limitedCMDPWR = lastOutputCMDPWR;
                    } else {
                        limitedCMDPWR = lastOutputCMDPWR;
                    }
                } else {
                    // Power is close to limit - hold current value
                    limitedCMDPWR = lastOutputCMDPWR;
                }
            } else {
                // Not outputting assist power - allow normal operation
                limitedCMDPWR = requestedCMDPWR_percent;
                lastOutputCMDPWR = limitedCMDPWR;
            }
        } else {
            // LiBCM disconnected - use conservative fixed limit with 60% ceiling
            uint8_t conservativeCeiling = 60; // Very conservative estimate for 13kW limit
            
            if (requestedCMDPWR_percent > JOYSTICK_NEUTRAL_MAX_PERCENT) {
                // Smoothly approach the conservative ceiling
                uint8_t targetCMDPWR = map(requestedCMDPWR_percent, 
                                          JOYSTICK_NEUTRAL_MAX_PERCENT, 100, 
                                          JOYSTICK_NEUTRAL_MAX_PERCENT, conservativeCeiling);
                
                // Smooth approach to target (no sudden jumps)
                if (abs(targetCMDPWR - lastOutputCMDPWR) > 2) {
                    if (targetCMDPWR > lastOutputCMDPWR) {
                        lastOutputCMDPWR += 1; // Gentle increase
                    } else {
                        lastOutputCMDPWR -= 1; // Gentle decrease
                    }
                } else {
                    lastOutputCMDPWR = targetCMDPWR;
                }
                
                limitedCMDPWR = lastOutputCMDPWR;
            } else {
                // In neutral or regen range - no limiting needed
                limitedCMDPWR = requestedCMDPWR_percent;
                lastOutputCMDPWR = limitedCMDPWR;
            }
        }
        
        // Apply smooth RPM-based scaling in transition zone
        if (currentRPM >= rpmLimitStart && currentRPM < rpmLimitEnd) {
            // Calculate where we are in the transition (0-100%)
            uint8_t rpmScalePercent = map(currentRPM, rpmLimitStart, rpmLimitEnd, 0, 100);
            
            // Calculate target based on RPM position
            uint16_t targetCMDPWR = limitedCMDPWR + 
                ((requestedCMDPWR_percent - limitedCMDPWR) * rpmScalePercent) / 100;
            
            // Slow approach to target to prevent overcurrent faults
            // Maximum 0.5% change per cycle when transitioning out of power limiting
            if (targetCMDPWR > lastOutputCMDPWR) {
                // Exiting power limited zone - be extra careful with increases
                if ((targetCMDPWR - lastOutputCMDPWR) > 1) {
                    lastOutputCMDPWR += 1; // Maximum 1% increase per cycle
                } else {
                    lastOutputCMDPWR = (uint8_t)targetCMDPWR; // Small change, allow it
                }
            } else if (targetCMDPWR < lastOutputCMDPWR) {
                // Entering power limited zone - can be faster for reductions
                if ((lastOutputCMDPWR - targetCMDPWR) > 2) {
                    lastOutputCMDPWR -= 2; // 2% reduction per cycle is OK
                } else {
                    lastOutputCMDPWR = (uint8_t)targetCMDPWR;
                }
            } else {
                lastOutputCMDPWR = (uint8_t)targetCMDPWR; // No change needed
            }
            
            limitedCMDPWR = lastOutputCMDPWR;
        }
    } else {
        // Not in low RPM limiting zone - allow normal operation but with slow transitions
        // This is to prevent IMA faults when exiting power limiting
        if (abs(requestedCMDPWR_percent - lastOutputCMDPWR) > 3) {
            // Large change requested - approach very gradually to prevent overcurrent faults
            if (requestedCMDPWR_percent > lastOutputCMDPWR) {
                // Increasing power - be extra cautious to prevent overcurrent
                lastOutputCMDPWR += 1; // Only 1% increase per cycle when exiting power limiting
            } else {
                // Decreasing power - can be slightly faster
                lastOutputCMDPWR -= 2; // 2% decrease per cycle is OK
            }
            limitedCMDPWR = lastOutputCMDPWR;
        } else {
            // Small change - allow it
            limitedCMDPWR = requestedCMDPWR_percent;
            lastOutputCMDPWR = limitedCMDPWR;
        }
    }
    
    #ifdef ENABLE_HIGH_RPM_ASSIST_LIMIT
    // Apply high RPM safety limiting with smooth transitions
    if (currentRPM >= RPM_ASSIST_CUTOFF) {
        // Complete assist cutoff above 5500 RPM - approach gradually
        if (lastOutputCMDPWR > JOYSTICK_NEUTRAL_NOM_PERCENT + 2) {
            lastOutputCMDPWR -= 2;
        } else {
            lastOutputCMDPWR = JOYSTICK_NEUTRAL_NOM_PERCENT;
        }
        limitedCMDPWR = lastOutputCMDPWR;
    }
    else if (currentRPM >= RPM_ASSIST_RAMPDOWN_START) {
        // Linear ramp down from 5200 to 5500 RPM
        uint8_t highRpmCeiling = map(currentRPM, RPM_ASSIST_RAMPDOWN_START, RPM_ASSIST_CUTOFF, 100, JOYSTICK_NEUTRAL_MAX_PERCENT);
        
        if (limitedCMDPWR > highRpmCeiling) {
            // Approach ceiling gradually
            if (limitedCMDPWR > highRpmCeiling + 2) {
                limitedCMDPWR = lastOutputCMDPWR - 2;
            } else {
                limitedCMDPWR = highRpmCeiling;
            }
            lastOutputCMDPWR = limitedCMDPWR;
        }
    }
    #endif
    
    return limitedCMDPWR;
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Power limiting function that applies ALL limits
uint8_t mcm_applyAllLimits(uint8_t requestedCMDPWR_percent, uint8_t mamode1_state)
{
    // Step 1: Apply clutch lockout first (highest priority)
    uint8_t clutchLimitedCMDPWR = mcm_applyClutchLockout(requestedCMDPWR_percent, mamode1_state);
    
    // Step 2: Apply RPM-based power limiting (only to assist mode)
    return mcm_applyRPMPowerLimiting(clutchLimitedCMDPWR, mamode1_state);
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Apply limits only to joystick-derived commands
uint8_t mcm_applyJoystickLimits(uint8_t requestedCMDPWR_percent, uint8_t mamode1_state)
{
    // Step 1: Apply clutch lockout first (highest priority) - only for joystick commands
    uint8_t clutchLimitedCMDPWR = mcm_applyClutchLockout(requestedCMDPWR_percent, mamode1_state);
    
    // Step 2: Apply RPM-based power limiting (only to assist mode)
    return mcm_applyRPMPowerLimiting(clutchLimitedCMDPWR, mamode1_state);
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Apply only RPM limits (no clutch lockout) for ECM commands
uint8_t mcm_applyECMLimits(uint8_t requestedCMDPWR_percent, uint8_t mamode1_state)
{
    // Only apply RPM-based power limiting (no clutch lockout for ECM commands)
    return mcm_applyRPMPowerLimiting(requestedCMDPWR_percent, mamode1_state);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void mcm_setMAMODE1_state (uint8_t newState  ) { gpio_setMCM_MAMODE1_percent(newState);   } //JTS2doNow: Redundant... remove
void mcm_setMAMODE2_state (uint8_t newState  ) { gpio_setMCM_MAMODE2_bool   (newState);   }

// Modified to apply clutch lockout at the very last moment
void mcm_setCMDPWR_percent(uint8_t newPercent) 
{ 
    // Get current conditions
    uint16_t currentRPM = engineSignals_getLatestRPM();
    bool clutchPressed = gpio_getClutchPosition();
    
    if (clutchPressed) {
        newPercent = 50; // Force neutral when clutch is pressed
    }
    
    gpio_setMCM_CMDPWR_percent(newPercent); 
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Apply power limiting based on RPM
uint8_t mcm_applyPowerLimiting(uint8_t requestedCMDPWR_percent, uint8_t mamode1_state)
{
    // This function is now just a wrapper for backwards compatibility
    return mcm_applyAllLimits(requestedCMDPWR_percent, mamode1_state);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void mcm_setAllSignals(uint8_t newState, uint16_t CMDPWR_percent)
{
    // Apply joystick-specific limits (includes clutch lockout + RPM limits)
    uint8_t limitedCMDPWR_percent = mcm_applyJoystickLimits((uint8_t)CMDPWR_percent, newState);
    
    mcm_setMAMODE1_state(newState);

    if(limitedCMDPWR_percent > 90) { limitedCMDPWR_percent = 90; }
    if(limitedCMDPWR_percent < 10) { limitedCMDPWR_percent = 10; }

    if     (newState == MAMODE1_STATE_IS_ASSIST) { mcm_setMAMODE2_state(MAMODE2_STATE_IS_ASSIST);        mcm_setCMDPWR_percent(limitedCMDPWR_percent); }
    else if(newState == MAMODE1_STATE_IS_REGEN)  { mcm_setMAMODE2_state(MAMODE2_STATE_IS_REGEN_STANDBY); mcm_setCMDPWR_percent(limitedCMDPWR_percent); }
    else if(newState == MAMODE1_STATE_IS_IDLE)   { mcm_setMAMODE2_state(MAMODE2_STATE_IS_REGEN_STANDBY); mcm_setCMDPWR_percent(limitedCMDPWR_percent); }
    else
    {
        ;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////

void mcm_passUnmodifiedSignals_fromECM(void)
{
    uint8_t ecmCMDPWR = ecm_getCMDPWR_percent();
    uint8_t ecmMAMODE1 = ecm_getMAMODE1_state();
    
    // Apply only RPM limits to ECM signals (NO clutch lockout for ECM commands)
    uint8_t limitedCMDPWR = mcm_applyECMLimits(ecmCMDPWR, ecmMAMODE1);
    
    mcm_setMAMODE1_state(ecmMAMODE1);
    mcm_setMAMODE2_state(ecm_getMAMODE2_state());
    mcm_setCMDPWR_percent(limitedCMDPWR);
}