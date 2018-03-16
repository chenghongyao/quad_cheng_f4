#include "sys.h"
#include "HAL_F4.h"

#include "QMath.h"
#include "InertialSensor_ICM20602.h"
#include "Compass_AK8975.h"
#include "AHRS_DCM.h"
#include "Parameters.h"
#include "Notify.h"
#include "state.h"
#include "Ano_DT.h"
#include "AttitudeControl.h"
#include "Motors.h"
#include "sbus.h"
#include "Attitude.h"
#include "failsafe.h"
#include "AHRS_Q.h"
#include "AverageFilter.h"

state_t state;
Parameters g;
static Notify notify;

static Scheduler& scheduler = hal.scheduler;
static InertialSensor_ICM20602 ins(hal.spi3, GPIO(PA15));
static Compass_AK8975 compass(hal.spi3, GPIO(PB14));

//static AHRS_DCM ahrs(ins,compass);
static AHRS_Q ahrs(ins, compass);
Motors motors(g.rcout_roll, g.rcout_pitch, g.rcout_throttle, g.rcout_yaw);

AttitudeControl attitude_control(ahrs, motors,
	g.p_angle_roll, g.p_angle_pitch, g.p_angle_yaw,
	g.pid_rate_roll, g.pid_rate_pitch, g.pid_rate_yaw);

extern sbus_t sbus;





static void stabilize_run(float dt)
{
	float target_roll, target_pitch;
	float target_yaw_rate;
//	float pilot_throttle_scaled;

	//δ������δ����
	if (!motors.armed() || g.rcin_throttle.control_in <= 0)	
	{
		attitude_control.relax_bf_rate_controller();			//��������������Ϊ��ǰ����
		attitude_control.set_yaw_target_to_current_heading();	//������������Ϊ��ǰ����
		attitude_control.set_throttle_out(0, false);			//�����������Ϊ0
		return;
	}
	
	//z��Ƕ�תΪ���ٶ� (*4.5)
	target_yaw_rate = get_pilot_desired_yaw_rate(g.rcin_yaw.control_in);//
	
	//get_pilot_desired_lean_angles(g.rcin_roll.control_in, g.rcin_pitch.control_in, target_roll, target_pitch);
	target_roll = g.rcin_roll.control_in;
	target_pitch = g.rcin_pitch.control_in;
	attitude_control.angle_ef_roll_pitch_rate_ef_yaw_smooth(
		target_roll, target_pitch,target_yaw_rate,dt, get_smoothing_gain());

	//pilot_throttle_scaled = get_pilot_desired_throttle(g.rcin_throttle.control_in);
	attitude_control.set_throttle_out(g.rcin_throttle.control_in,true);		//��������

}


//�����������Ƿ�����
void pre_arm_check()		
{
	if (motors.armed())return;
	if (state.pre_arm_check)return;

	//����������λ��?

	//������У��ͨ��?
	if (!ins.calibrated())return;//
	//
	set_pre_arm_check(true);
}
//��⵱ǰ����״̬
bool arm_check()
{
	if (motors.armed())return true;
	return true;
}
//����
bool init_arm_motor()
{
	ahrs.set_fast_gains(false);
	ahrs.set_armed(true);
	motors.armed();

	return true;
}
//����
bool init_disarm_motor()
{
	ahrs.set_fast_gains(true);
	ahrs.set_armed(false);
	motors.disarm();

	return true;
}


//////////////////////////////////////////////////////////////////////////
//loop
//////////////////////////////////////////////////////////////////////////
//400Hz(2.5ms)
void fast_loop()
{
	float deltat = 2.5f / 1000.0f;
	ahrs.update(deltat);		//����ŷ����
	attitude_control.rate_control_run(deltat);			//atti
	motors.output();									//output
	stabilize_run(deltat);
}
//100Hz(10ms)
void rc_loop(void)
{
	static uint32_t last_update_ms = 0;
	uint32_t tnow_ms = millis();
	if (sbus.state == SBUS_AVAILABLE)
	{
		sbus.state = SBUS_USING;
		g.rcin_roll.set_radio(sbus.channel.ch1);
		g.rcin_pitch.set_radio(sbus.channel.ch2);
		g.rcin_throttle.set_radio(sbus.channel.ch3);
		g.rcin_yaw.set_radio(sbus.channel.ch4);
		sbus.state = SBUS_EMPTY;

		last_update_ms = tnow_ms;
		set_throttle_failsafe(g.rcin_throttle.radio_in);		//ʧ�����ż��,sbusӲ��֧��
		set_throttle_zero(g.rcin_throttle.control_in);			//0���ż��
	}
	else
	{
		if(motors.armed())	//����״̬��һ��ʱ��û�����룬����ʧ�ر�����Ĭ��500ms
		{
			uint32_t elapsed = tnow_ms - last_update_ms;
			if (elapsed >= FS_RADIO_TIMEOUT_MS)
			{										
				failsafe_set_radio(true);			//ʧ�ر���
			}
		}
		
	
	}
}
//������С,��������->����
//������С,��������->����
//10hz(100ms)
void arm_check_loop(void)
{
#define  ARM_DELAY		20			//2s
#define  DISARM_DELAY	20			//2s

	static int16_t arming_count = 0;
	if (g.rcin_throttle.control_in > 0)//���ŷ�0,�˳�
	{
		arming_count = 0;
		return;
	}

	int16_t tmp = g.rcin_yaw.control_in;		//-4500 ~4500
	if (tmp > 4000)		//����
	{
		if (arming_count < ARM_DELAY)
			arming_count++;

		if (arming_count == ARM_DELAY && !motors.armed())
		{
			pre_arm_check();
			if (state.pre_arm_check && arm_check())
			{
				if (!init_arm_motor())
				{
					arming_count = 0;
				}
			}
			else
			{
				arming_count = 0;
			}
		}
	}
	else if (tmp < -4000)		//����
	{
		if (arming_count < DISARM_DELAY)
			arming_count++;

		if (arming_count == DISARM_DELAY && motors.armed())
		{
			init_disarm_motor();
			arming_count = 0;
		}
	}
	else
	{
		arming_count = 0;
	}
}


//
void display_loop()
{
	Vector3f acc = ins.get_accel();
 	Vector3f gyro = ins.get_gyro();
	static Vector3f mag;
		ANO_DT_Send_Senser(acc.x * 100, acc.y * 100, acc.z * 100, gyro.x * 100, gyro.y * 100, gyro.z * 100, 0, 0, 0);
	//ANO_DT_Send_Status(ahrs.roll_sensor/100.0f, ahrs.pitch_sensor/100.0f, ahrs.yaw_sensor/100.0f, 0, 0, 0);
	if(compass.get_available())
	{
		mag = compass.get_mag();
		compass.set_available(false);
	}
	//float leng = mag.length();
	//printf("x=%f,y=%f,z=%f,len=%f\r\n", mag.x, mag.y, mag.z, mag.length());
	//ANO_DT_Send_Senser(acc.x * 100, acc.y * 100, acc.z * 100, gyro.x * 100, gyro.y * 100, gyro.z * 100, mag.x*100,mag.y*100,mag.z*100);

}
//50Hz,20ms
void throttle_loop()
{


}
//10Hz,100ms
void notify_loop(){
	notify.update();
}

void compass_update_loop()
{
	compass.update();
	compass.start();
}
//////////////////////////////////////////////////////////////////////////
//setup
////////////////////////////////////////////////////////////////////////////////////////////////	
void init_rcin()
{
	g.rcin_roll(SBUS_MIN, SBUS_MID, SBUS_MAX);
	g.rcin_roll.set_dead_zone(30);
	g.rcin_roll.set_reverse(1);
	g.rcin_roll.set_angle(RC_RPY_INPUT_MAX);


	g.rcin_pitch(SBUS_MIN, SBUS_MID, SBUS_MAX);
	g.rcin_pitch.set_dead_zone(30);
	g.rcin_pitch.set_reverse(1);
	g.rcin_pitch.set_angle(RC_RPY_INPUT_MAX);

	g.rcin_yaw(SBUS_MIN, SBUS_MID, SBUS_MAX);
	g.rcin_yaw.set_dead_zone(30);
	g.rcin_yaw.set_reverse(1);
	g.rcin_yaw.set_angle(RC_RPY_INPUT_MAX);

	g.rcin_throttle(SBUS_MIN, SBUS_MID, SBUS_MAX);
	g.rcin_throttle.set_dead_zone(50);
	g.rcin_throttle.set_reverse(1);
	g.rcin_throttle.set_range(g.throttle_min, g.throttle_max);

}

void init_rcout()
{
	g.rcout_throttle.config_radio(1100,1500,1900);
	g.rcout_throttle.set_reverse(1);
	g.rcout_throttle.set_range(g.throttle_min, g.throttle_max);

	g.rcout_roll.config_radio(1100, 1500, 1900);
	g.rcout_roll.set_reverse(1);
	g.rcout_roll.set_type(RCOut::TYPE_ANGLE_RAW);

	g.rcout_pitch.config_radio(1100, 1500, 1900);
	g.rcout_pitch.set_reverse(1);
	g.rcout_pitch.set_type(RCOut::TYPE_ANGLE_RAW);

	g.rcout_yaw.config_radio(1100, 1500, 1900);
	g.rcout_yaw.set_reverse(1);
	g.rcout_yaw.set_type(RCOut::TYPE_ANGLE_RAW);
}

void init_motors()
{
	//x�ᣬ��ͷ����Ϊ0��
	motors.set_throttle_max(g.throttle_max);
	motors.set_throttle_min(g.throttle_min);
	motors.set_throttle_hover(g.throttle_mid);


	
	//===================4��================================
	motors.remove_all_motors();
	motors.add_motor(MOTORS_MOT_1, 45, MOTORS_YAW_FACTOR_CCW);	//����
	motors.add_motor(MOTORS_MOT_2, -135, MOTORS_YAW_FACTOR_CCW);//����
	motors.add_motor(MOTORS_MOT_3, -45, MOTORS_YAW_FACTOR_CW);	//����
	motors.add_motor(MOTORS_MOT_4, 135, MOTORS_YAW_FACTOR_CW);	//����

}

void startup_ground()
{
	ins.init();
	compass.init();
	compass.start();

	ahrs.init();
	ahrs.reset_gyro_drift();		//��������ǻ���Ư��
	ahrs.set_fast_gains(true);			//���ٽ���,P*8,������ȡ��
}


void setup()
{
	init_rcin();
	init_rcout();
	init_motors();
	notify.init();
	
	startup_ground();
	pre_arm_check();
	state.initialised = true;
}
//////////////////////////////////////////////////////////////////////////
const scheduler_tasks_t scheduler_tasks[] ={
	{ fast_loop,		2500,	1000 },	//400Hz,2.5ms,��̬����,����
	{ display_loop,		2500,	1000 },	//400Hz,2.5ms,
	{ rc_loop,			10000,	1000 },	//100Hz,10ms,���ջ�������
	{ arm_check_loop,	100000,	1000 },	//10Hz,100ms,ң�ؽ������μ��
	{ throttle_loop,	20000,	1000 },	//50Hz,20ms,
	{ notify_loop,		100000,	100	 },//10Hz,100ms
	{ compass_update_loop,20000,	1000 },
};
int main(void)
{
	uint32_t i = 0;
	hal.Setup();

// 	while (1)
// 	{
// 		printf("USART1_TEST=%d\r\n", i);
// 		if (++i == 101)i = 1;
// 		delay_ms(100);
// 	}
	printf("start\r\n");
	ins.init();
	compass.init();
// 	setup();
// 	scheduler.init(scheduler_tasks, sizeof(scheduler_tasks) / sizeof(scheduler_tasks[0]));
// 	scheduler.run();
	while (1);
}
