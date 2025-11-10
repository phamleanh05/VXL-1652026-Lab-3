/*
 * trafficlight.c
 *
 *  Created on: Nov 8, 2025
 *      Author: ATPHOME
 */

#include "trafficlight.h"

void setTrafficLight1(int state) {
    switch (state) {
	case RED:
		HAL_GPIO_WritePin(GPIOA, LED_RED_1_Pin, RESET);
		HAL_GPIO_WritePin(GPIOA, LED_YELLOW_1_Pin | LED_GREEN_1_Pin, SET);
		break;
	case YELLOW:
		HAL_GPIO_WritePin(GPIOA, LED_YELLOW_1_Pin, RESET);
		HAL_GPIO_WritePin(GPIOA, LED_RED_1_Pin | LED_GREEN_1_Pin, SET);
		break;
	case GREEN:
		HAL_GPIO_WritePin(GPIOA, LED_GREEN_1_Pin, RESET);
		HAL_GPIO_WritePin(GPIOA, LED_RED_1_Pin | LED_YELLOW_1_Pin, SET);
		break;
    }
}

void setTrafficLight2(int state) {
    switch (state) {
	case RED:
		HAL_GPIO_WritePin(GPIOA, LED_RED_2_Pin, RESET);
		HAL_GPIO_WritePin(GPIOA, LED_YELLOW_2_Pin | LED_GREEN_2_Pin, SET);
		break;
	case YELLOW:
		HAL_GPIO_WritePin(GPIOA, LED_YELLOW_2_Pin, RESET);
		HAL_GPIO_WritePin(GPIOA, LED_RED_2_Pin | LED_GREEN_2_Pin, SET);
		break;
	case GREEN:
		HAL_GPIO_WritePin(GPIOA, LED_GREEN_2_Pin, RESET);
		HAL_GPIO_WritePin(GPIOA, LED_RED_2_Pin | LED_YELLOW_2_Pin, SET);
		break;
    }
}

void update7SEG_light1(int counter) {
    int tens = counter / 10;
    int units = counter % 10;

	if (switch_seg == 0) {
		HAL_GPIO_WritePin(GPIOA, EN0_Pin, RESET);
		HAL_GPIO_WritePin(GPIOA, EN1_Pin, SET);

		display7SEG_traffic_light1(tens);

	} else {
		HAL_GPIO_WritePin(GPIOA, EN0_Pin, SET);
		HAL_GPIO_WritePin(GPIOA, EN1_Pin, RESET);

		display7SEG_traffic_light1(units);
	}
}

void update7SEG_light2(int counter) {
    int tens = counter / 10;
    int units = counter % 10;

	if (switch_seg == 0) {
		HAL_GPIO_WritePin(GPIOB, EN2_Pin, RESET);
		HAL_GPIO_WritePin(GPIOB, EN3_Pin, SET);

		display7SEG_traffic_light2(tens);

	} else {
		HAL_GPIO_WritePin(GPIOB, EN2_Pin, SET);
		HAL_GPIO_WritePin(GPIOB, EN3_Pin, RESET);

		display7SEG_traffic_light2(units);
	}
}


int getNextState(int currentState) {
    switch(currentState) {
        case RED: return GREEN;
        case GREEN: return YELLOW;
        case YELLOW: return RED;
        default: return RED;
    }
}

int getDuration(int state) {
    switch(state) {
        case RED: return RED_DURATION;
        case GREEN: return GREEN_DURATION;
        case YELLOW: return YELLOW_DURATION;
        default: return RED_DURATION;
    }
}

void init(void){
	traffic_state1 = RED;
	traffic_state2 = GREEN;
	counter1 = RED_DURATION;
	counter2 = GREEN_DURATION;
	setTrafficLight1(traffic_state1);
	setTrafficLight2(traffic_state2);
}

void trafficLightRun(void) {
	if (timer2_flag == 1) {
		setTimer2(50);
		switch_seg = 1 - switch_seg;
		update7SEG_light1(counter1);
		update7SEG_light2(counter2);
	}
    if (timer1_flag == 1) {
        setTimer1(100);
        counter1--;
        counter2--;

        setTrafficLight1(traffic_state1);
        setTrafficLight2(traffic_state2);
		update7SEG_light1(counter1);
		update7SEG_light2(counter2);

        if (counter1 <= 0) {
            traffic_state1 = getNextState(traffic_state1);
            counter1 = getDuration(traffic_state1);
            setTrafficLight1(traffic_state1);
            update7SEG_light1(counter1);
        }

        if (counter2 <= 0) {
            traffic_state2 = getNextState(traffic_state2);
            counter2 = getDuration(traffic_state2);
            setTrafficLight2(traffic_state2);
            update7SEG_light2(counter2);
        }
    }
}

void manualLight(void){
	if(traffic_state1 == YELLOW){
		traffic_state1 = GREEN;
		setTrafficLight1(traffic_state1);
	}
	if(traffic_state2 == YELLOW){
		traffic_state2 = GREEN;
		setTrafficLight2(traffic_state2);
	}
	if(isButton2Pressed() == 1){
		if(traffic_state1 == RED){
			traffic_state1 = GREEN;
			traffic_state2 = RED;
		} else if(traffic_state1 == GREEN) {
			traffic_state1 = RED;
			traffic_state2 = GREEN;
		}
		setTrafficLight1(traffic_state1);
		setTrafficLight2(traffic_state2);
	}
}

void ConfigTimeREDLight(void){
	if (isButton2Pressed() == 1){
		temp_red_time++;
		if(temp_red_time > 99){
			temp_red_time = 1;
		}
	}

	if (timer2_flag == 1) {
		setTimer2(50);
		switch_seg = 1 - switch_seg;
		setTrafficLight1(RED);
		setTrafficLight2(RED);
		update7SEG_light1(temp_red_time);
		update7SEG_light2(temp_red_time);
	}
}

void setTimeREDLight() {
	ConfigTimeREDLight();
	if (isButton3Pressed() == 1){
		RED_DURATION = temp_red_time;
		config_done_flag = 1;
	}
}

void ConfigTimeGreenLight(void){
	if (isButton2Pressed() == 1){
		temp_green_time++;
		if(temp_green_time > RED_DURATION - YELLOW_DURATION){
			temp_green_time = 1;
		}
	}

	if (timer2_flag == 1) {
		setTimer2(50);
		switch_seg = 1 - switch_seg;
		setTrafficLight1(GREEN);
		setTrafficLight2(GREEN);
		update7SEG_light1(temp_green_time);
		update7SEG_light2(temp_green_time);
	}
}

void setTimeGreenLight() {
	ConfigTimeGreenLight();
	if (isButton3Pressed() == 1){
		GREEN_DURATION = temp_green_time;
		YELLOW_DURATION = RED_DURATION - GREEN_DURATION;
		config_done_flag = 1;
	}
}
