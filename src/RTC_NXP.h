/** RTC operation library for Arduino
 *
 *  @author Tedd OKANO
 *
 *  Released under the MIT license License
 */

#ifndef ARDUINO_LED_DRIVER_NXP_ARD_H
#define ARDUINO_LED_DRIVER_NXP_ARD_H

#include <Arduino.h>
#include <stdint.h>
#include <time.h>

#include <I2C_device.h>
#include <SPI.h>

/** RTC_NXP class
 *	
 *  @class RTC_NXP
 *
 *	RTC_NXP class is a base class for all RTC drivers
 *	All actual device class will be derived from this class
 */

class RTC_NXP
{
public:
	/** Arduino shield board evaluation board selection descriptor */
	enum board {
		NONE,
		ARDUINO_SHIELD,
	};
	/** Alarm setting digit selection descriptor */
	enum alarm_setting {
		SECOND,
		MINUTE,
		HOUR,
		DAY,
		WEEKDAY,
	};

	/** Constructor */
	RTC_NXP();

	/** Destructor */
	virtual ~RTC_NXP();
	
	/** Initializer (pure virtual method) */
	virtual void begin( void )	= 0;
	
	/** time
	 * 
	 *	"time()" in "time.h" compatible method for RTC
	 *
	 * @param tp pointer to time_t variable
	 * @return time_t value of current time
	 */
	time_t time( time_t* tp );
	
	/** set (pure virtual method)
	 * 
	 * @param now_tm struct to set calendar and time in RTC
	 */
	virtual void set( struct tm* now_tm )	= 0;

	/** Detector for oscillation stop (pure virtual method)
	 * 
	 * @return true, if the OSF (Oscillator Stop Flag) is set
	 */
	virtual bool oscillator_stop( void )	= 0;
	
	/** Alarm setting (pure virtual method)
	 * 
	 * @param digit to specify which parameter to set: SECOND, MINUTE, HOUR, DAY, WEEKDAY in 'enum alarm_setting'. Set 0x80 to disabling
	 */
	virtual void alarm( alarm_setting digit, int val )	= 0;

	/** Alarm clearing (pure virtual method)
	 */
	virtual void alarm_clear( void )	= 0;

	/** Alarm interrupt disable (pure virtual method)
	 */
	virtual void alarm_disable( void )	= 0;

	/** Interrupt clear (pure virtual method)
	 */
	virtual uint8_t int_clear( void )	= 0;

protected:
	/** rtc_time (pure virtual method)
	 * 
	 * @return time_t returns RTC time in time_t format
	 */
	virtual time_t rtc_time( void )	= 0;
		
	/** Class method for BCD to int conversion
	 * 
	 * @param v BCD value
	 * @return integer 
	 */
	static uint8_t	bcd2dec( uint8_t v );

	/** Class method for int to BCD conversion
	 * 
	 * @param v integer
	 * @return BCD value
	 */
	static uint8_t	dec2bcd( uint8_t v );
};


/** PCF2131_base class
 *	
 *	A base class for PCF2131
 *	Implementing register operation with abstarcted interface
 *
 *  @class PCF2131_base
 */

class PCF2131_base : public RTC_NXP
{
public:
	/** register addresses */
	enum reg_num {
		Control_1, Control_2, Control_3, Control_4, Control_5,
		SR_Reset,
		_100th_Seconds, Seconds, Minutes,Hours, Days, Weekdays, Months, Years,
		Second_alarm, Minute_alarm, Hour_alarm, Day_alarm, Weekday_alarm,
		CLKOUT_ctl,
		Timestp_ctl1, Sec_timestp1, Min_timestp1, Hour_timestp1, Day_timestp1, Mon_timestp1, Year_timestp1,
		Timestp_ctl2, Sec_timestp2, Min_timestp2, Hour_timestp2, Day_timestp2, Mon_timestp2, Year_timestp2,
		Timestp_ctl3, Sec_timestp3, Min_timestp3, Hour_timestp3, Day_timestp3, Mon_timestp3, Year_timestp3,
		Timestp_ctl4, Sec_timestp4, Min_timestp4, Hour_timestp4, Day_timestp4, Mon_timestp4, Year_timestp4,
		Aging_offset,
		INT_A_MASK1, INT_A_MASK2, INT_B_MASK1, INT_B_MASK2,
		Watchdg_tim_ctl, Watchdg_tim_val
	};
	/** Periodic interrupt selection descriptor */
	enum periodic_int_select {
		DISABLE,
		EVERY_SECOND,
		EVERY_MINUTE,
	};
	/** Timestamp setting descriptor */
	enum timestanp_setting {
		LAST,
		FIRST,
	};

	/** Constructor */
	PCF2131_base();

	/** Destructor */
	virtual ~PCF2131_base();
	
	/** Initializer */
	void begin( void );

	/** Detector for oscillation stop
	 * 
	 * @return true, if the OSF (Oscillator Stop Flag) is set
	 */
	bool oscillator_stop( void );

	/** time
	 * 
	 * @return time_t value of current time
	 */
	time_t rtc_time( void );

	/** set
	 * 
	 * @param now_tm struct to set calendar and time in RTC
	 */
	void set( struct tm* now_tm );
	
	
	/** Alarm setting
	 * 
	 * @param digit to specify which parameter to set: SECOND, MINUTE, HOUR, DAY, WEEKDAY in 'enum alarm_setting'
	 * @param val Set 0x80 to disabling
	 */
	void alarm( alarm_setting digit, int val );

	/** Alarm setting
	 * 
	 * @param digit to specify which parameter to set: SECOND, MINUTE, HOUR, DAY, WEEKDAY in 'enum alarm_setting'
	 * @param val Set 0x80 to disabling
	 * @param int_sel Interrupt output selector. ) for INT_A, 1 for INT_B
	 */
	void alarm( alarm_setting digit, int val, int int_sel );

	/** Alarm interrupt disable
	 */
	void alarm_clear( void );

	/** Interrupt clear
	 */
	void alarm_disable( void );
	
	/** Timestamp setting
	 * 
	 * @param num timestamp number: 1~4
	 * @param ts_setting event recording option. Choose LAST or FIRST in 'enum timestanp_setting'
	 * @param int_sel Interrupt output selector. ) for INT_A, 1 for INT_B
	 */
	void timestamp( int num, timestanp_setting ts_setting, int int_sel = 0 );

	/** Getting timestamp info
	 * 
	 * @param num timestamp number: 1~4
	 * @return time_t
	 */
	time_t timestamp( int num );
	
	/** Interrupt clear
	 */
	uint8_t int_clear( void );

	/** Interrupt clear
	 */
	uint8_t int_clear( uint8_t* state_p );

	/** Enabling every second/minute interrupt
	 * 
	 * @param sel choose DISABLE, EVERY_SECOND or EVERY_MINUTE in 'enum periodic_int_select'
	 * @param int_sel Interrupt output selector. ) for INT_A, 1 for INT_B
	 */
	void periodic_interrupt_enable( periodic_int_select sel, int int_sel = 0 );

protected:
	/** Proxy method for interface  (pure virtual method) */
	virtual void _reg_w( uint8_t reg, uint8_t *vp, int len )	= 0;

	/** Proxy method for interface  (pure virtual method) */
	virtual void _reg_r( uint8_t reg, uint8_t *vp, int len )	= 0;

	/** Proxy method for interface  (pure virtual method) */
	virtual void _reg_w( uint8_t reg, uint8_t val )	= 0;

	/** Proxy method for interface  (pure virtual method) */
	virtual uint8_t _reg_r( uint8_t reg )	= 0;

	/** Proxy method for interface  (pure virtual method) */
	virtual void _bit_op8( uint8_t reg, uint8_t mask, uint8_t val )	= 0;

private:
	const int int_mask_reg[ 2 ][ 2 ]	= {
		{ INT_A_MASK1, INT_A_MASK2, },
		{ INT_B_MASK1, INT_B_MASK2, },
	};
};


/** PCF2131_I2C class
 *	
 *	PCF2131 class driver using I2C interface
 *
 *  @class PCF2131_I2C
 */

class PCF2131_I2C : public PCF2131_base, public I2C_device
{
public:
	/** Constructor */
	PCF2131_I2C( uint8_t i2c_address = (0xA6 >> 1) );

	/** Destructor */
	virtual ~PCF2131_I2C();
	
private:
	/** Proxy method for interface */
	void _reg_w( uint8_t reg, uint8_t *vp, int len );

	/** Proxy method for interface */
	void _reg_r( uint8_t reg, uint8_t *vp, int len );

	/** Proxy method for interface */
	void _reg_w( uint8_t reg, uint8_t val );

	/** Proxy method for interface */
	uint8_t _reg_r( uint8_t reg );

	/** Proxy method for interface */
	void _bit_op8( uint8_t reg, uint8_t mask, uint8_t val );
};


/** SPI_for_RTC class
 *	
 *	SPI interface operation for RTC devices
 *
 *  @class SPI_for_RTC
 */

class SPI_for_RTC
{
public:
	/** Send data
	 * 
	 * @param data pointer to data buffer
	 * @param size data size
	 * @param stop option: generating STOP-condition after transaction. Defailt: true
	 * @return transferred data size
	 */
	void txrx( uint8_t *data, uint16_t size );

	/** Multiple register write
	 * 
	 * @param reg register index/address/pointer
	 * @param data pointer to data buffer
	 * @param size data size
	 * @return transferred data size
	 */
	void reg_w( uint8_t reg_adr, uint8_t *data, uint16_t size );

	/** Single register write
	 * 
	 * @param reg register index/address/pointer
	 * @param data pointer to data buffer
	 * @param size data size
	 * @return transferred data size
	 */
	void reg_w( uint8_t reg_adr, uint8_t data );

	/** Multiple register read
	 * 
	 * @param reg register index/address/pointer
	 * @param data pointer to data buffer
	 * @param size data size
	 * @return transferred data size
	 */
	void reg_r( uint8_t reg_adr, uint8_t *data, uint16_t size );

	/** Single register read
	 * 
	 * @param reg register index/address/pointer
	 * @return read data
	 */
	uint8_t	reg_r( uint8_t reg_adr );

	/** Register write, 8 bit
	 *
	 * @param reg register index/address/pointer
	 * @param val data value
	 */
	void write_r8( uint8_t reg, uint8_t val );

	/** Register read, 8 bit
	 *
	 * @param reg register index/address/pointer
	 * @return data value
	 */
	uint8_t read_r8( uint8_t reg );

	/** Register overwriting with bit-mask
	 *	
	 *	Register can be updated by bit level
	 *
	 * @param reg register index/address/pointer
	 * @param mask bit-mask to protect overwriting
	 * @param value value to overwrite
	 */
	void bit_op8(  uint8_t reg,  uint8_t mask,  uint8_t value );
};

/** PCF2131_SPI class
 *	
 *	PCF2131 class driver using SPI interface
 *
 *  @class PCF2131_SPI
 */

class PCF2131_SPI : public PCF2131_base, public SPI_for_RTC
{
public:
	/** Constructor */
	PCF2131_SPI();

	/** Destructor */
	virtual ~PCF2131_SPI();
	
private:
//	void txrx( uint8_t *data, uint16_t size );

	/** Proxy method for interface */
	void _reg_w( uint8_t reg, uint8_t *vp, int len );

	/** Proxy method for interface */
	void _reg_r( uint8_t reg, uint8_t *vp, int len );

	/** Proxy method for interface */
	void _reg_w( uint8_t reg, uint8_t val );

	/** Proxy method for interface */
	uint8_t _reg_r( uint8_t reg );

	/** Proxy method for interface */
	void _bit_op8( uint8_t reg, uint8_t mask, uint8_t val );
};

/** PCF85063A class
 *	
 *	PCF85063A class driver
 *
 *  @class PCF85063A
 */

class PCF85063_base : public RTC_NXP\
{
public:
	enum reg_num {
		Control_1, Control_2,
		Offset,
		RAM_byte,
		Seconds, Minutes, Hours, Days, Weekdays, Months, Years,
		Second_alarm, Minute_alarm, Hour_alarm, Day_alarm, Weekday_alarm,
		Timer_value, Timer_mode
	};
	/** Constructor */
	PCF85063_base();

	/** Destructor */
	virtual ~PCF85063_base();
	
	/** Initializer */
	void begin( void );
	
	/** set
	 * 
	 * @param now_tm struct to set calendar and time in RTC
	 */
	void set( struct tm* now_tm );

	/** Detector for oscillation stop
	 * 
	 * @return true, if the OSF (Oscillator Stop Flag) is set
	 */
	bool oscillator_stop( void );
	
	/** Alarm setting
	 * 
	 * @param digit to specify which parameter to set: SECOND, MINUTE, HOUR, DAY, WEEKDAY in 'enum alarm_setting'. Set 0x80 to disabling
	 */
	void alarm( alarm_setting digit, int val );

	/** Alarm clearing
	 */
	void alarm_clear( void );

	/** Alarm interrupt disable
	 */
	void alarm_disable( void );

	/** Interrupt clear
	 */
	uint8_t int_clear( void );

	
	/** Timer setting
	 * 
	 * @param period timer interval in second
	 * @return actual timer set value in second
	 */
	float timer( float period );

protected:
	/** rtc_time
	 * 
	 * @return time_t returns RTC time in time_t format
	 */
	time_t rtc_time( void );

	/** Proxy method for interface  (pure virtual method) */
	virtual void _reg_w( uint8_t reg, uint8_t *vp, int len )	= 0;

	/** Proxy method for interface  (pure virtual method) */
	virtual void _reg_r( uint8_t reg, uint8_t *vp, int len )	= 0;

	/** Proxy method for interface  (pure virtual method) */
	virtual void _reg_w( uint8_t reg, uint8_t val )	= 0;

	/** Proxy method for interface  (pure virtual method) */
	virtual uint8_t _reg_r( uint8_t reg )	= 0;

	/** Proxy method for interface  (pure virtual method) */
	virtual void _bit_op8( uint8_t reg, uint8_t mask, uint8_t val )	= 0;
};

class PCF85063A : public PCF85063_base, public I2C_device
{
public:
	PCF85063A( uint8_t i2c_address = (0xA2 >> 1) );

	/** Destructor */
	virtual ~PCF85063A();
	
private:
	/** Proxy method for interface */
	void _reg_w( uint8_t reg, uint8_t *vp, int len );

	/** Proxy method for interface */
	void _reg_r( uint8_t reg, uint8_t *vp, int len );

	/** Proxy method for interface */
	void _reg_w( uint8_t reg, uint8_t val );

	/** Proxy method for interface */
	uint8_t _reg_r( uint8_t reg );

	/** Proxy method for interface */
	void _bit_op8( uint8_t reg, uint8_t mask, uint8_t val );
};

class PCF85063TP : public PCF85063A
{
public:
	PCF85063TP( uint8_t i2c_address = (0xA2 >> 1) );

	/** Destructor */
	virtual ~PCF85063TP();
	
	void alarm( alarm_setting digit, int val );

	/** Alarm clearing
	 *	Method overriding to disable this feature (Hardware is not supporting)
	 */
	void alarm_clear( void );

	/** Alarm interrupt disable
	 *	Method overriding to disable this feature (Hardware is not supporting)
	 */
	void alarm_disable( void );

	/** Interrupt clear
	 *	Method overriding to disable this feature (Hardware is not supporting)
	 */
	uint8_t int_clear( void );

	
	/** Timer setting
	 *	Method overriding to disable this feature (Hardware is not supporting)
	 */
	float timer( float period );

};

class ForFutureExtention : public RTC_NXP, public I2C_device
{
public:
	/** Constructor */
	ForFutureExtention();

	/** Destructor */
	virtual ~ForFutureExtention();
	
	/** Initializer */
	void begin( void );
	
	/** set
	 * 
	 * @param now_tm struct to set calendar and time in RTC
	 */
	void set( struct tm* now_tm );

	/** Detector for oscillation stop
	 * 
	 * @return true, if the OSF (Oscillator Stop Flag) is set
	 */
	bool oscillator_stop( void );
	
	/** Alarm setting
	 * 
	 * @param digit to specify which parameter to set: SECOND, MINUTE, HOUR, DAY, WEEKDAY in 'enum alarm_setting'. Set 0x80 to disabling
	 */
	void alarm( alarm_setting digit, int val );

	/** Alarm clearing
	 */
	void alarm_clear( void );

	/** Alarm interrupt disable
	 */
	void alarm_disable( void );

	/** Interrupt clear
	 */
	uint8_t int_clear( void );

protected:
	/** rtc_time
	 * 
	 * @return time_t returns RTC time in time_t format
	 */
	time_t rtc_time( void );
};


#endif //	ARDUINO_LED_DRIVER_NXP_ARD_H