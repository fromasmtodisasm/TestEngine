-- : ./AI/Packs/Sound/coreworkerD.lua
-- TEMPLATE
-- DO NOT MODIFY THIS TEMPLATE
--
-- This template has all the readability signals currently in use.  
-- Change the <Template> string to the pack name
-- 
-- To switch between alternate sound responses add a new set of 
-- bracketed parameters, making sure there is a comma after the bracket eg,
--				{
--				PROBABILITY = 500,
--				soundFile = "SOUNDS/<full path to wav file>",
--				Volume = 175,
--				min = 12,
--				max = 300,
--				sound_unscalable = 1,
--				},
-- Probability determines how often a wave is played in response to the signal
-- To allow for possibility of no sound in response to a signal, sounds should sum
-- to less than 1000. eg. if you dont want sound to be played every single time 
-- player receives an order might have total PROBABILITY for all sounds = 500.
--------------------------------------------------
--    Created By: <your_name>
--   Description: <short_description>
--------------------------	
SOUNDPACK.coreworkerD = {
	
	ORDER_RECEIVED = {
				{
				PROBABILITY = 100,
				soundFile = "Languages/voicepacks/coreworkerD/affirmative_1_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 100,
				soundFile = "Languages/voicepacks/coreworkerD/affirmative_2_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 100,
				soundFile = "Languages/voicepacks/coreworkerD/affirmative_3_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 100,
				soundFile = "Languages/voicepacks/coreworkerD/affirmative_4_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 100,
				soundFile = "Languages/voicepacks/coreworkerD/affirmative_5_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 100,
				soundFile = "Languages/voicepacks/coreworkerD/affirmative_6_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 100,
				soundFile = "Languages/voicepacks/coreworkerD/affirmative_7_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 100,
				soundFile = "Languages/voicepacks/coreworkerD/affirmative_8_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 100,
				soundFile = "Languages/voicepacks/coreworkerD/affirmative_9_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 100,
				soundFile = "Languages/voicepacks/coreworkerD/affirmative_10_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
			},
			
	CALL_ALARM = {
				{
				PROBABILITY = 200,
				soundFile = "Languages/voicepacks/coreworkerD/alarm_alone_1_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 200,
				soundFile = "Languages/voicepacks/coreworkerD/alarm_alone_2_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 200,
				soundFile = "Languages/voicepacks/coreworkerD/alarm_alone_3_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 200,
				soundFile = "Languages/voicepacks/coreworkerD/alarm_alone_4_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 200,
				soundFile = "Languages/voicepacks/coreworkerD/alarm_alone_5_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
			},
			
	CALL_ALARM_GROUP = {
				{
				PROBABILITY = 333,
				soundFile = "Languages/voicepacks/coreworkerD/alarm_group_1_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 333,
				soundFile = "Languages/voicepacks/coreworkerD/alarm_group_2_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 333,
				soundFile = "Languages/voicepacks/coreworkerD/alarm_group_3_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
			},
			
	FIRST_HOSTILE_CONTACT = {
				{
				PROBABILITY = 125,
				soundFile = "Languages/voicepacks/coreworkerD/alert_to_combat_al_1_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 125,
				soundFile = "Languages/voicepacks/coreworkerD/alert_to_combat_al_2_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 125,
				soundFile = "Languages/voicepacks/coreworkerD/alert_to_combat_al_3_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 125,
				soundFile = "Languages/voicepacks/coreworkerD/alert_to_combat_al_4_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 125,
				soundFile = "Languages/voicepacks/coreworkerD/alert_to_combat_al_5_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 125,
				soundFile = "Languages/voicepacks/coreworkerD/alert_to_combat_al_6_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 125,
				soundFile = "Languages/voicepacks/coreworkerD/alert_to_combat_al_7_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 125,
				soundFile = "Languages/voicepacks/coreworkerD/alert_to_combat_al_8_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
			},
			
	FIRST_HOSTILE_CONTACT_GROUP = {
				{
				PROBABILITY = 166,
				soundFile = "Languages/voicepacks/coreworkerD/alert_to_combat_gp_1_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 166,
				soundFile = "Languages/voicepacks/coreworkerD/alert_to_combat_gp_2_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 166,
				soundFile = "Languages/voicepacks/coreworkerD/alert_to_combat_gp_3_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 166,
				soundFile = "Languages/voicepacks/coreworkerD/alert_to_combat_gp_4_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 166,
				soundFile = "Languages/voicepacks/coreworkerD/alert_to_combat_gp_5_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 166,
				soundFile = "Languages/voicepacks/coreworkerD/alert_to_combat_gp_6_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
			},
			
	INTERESTED_TO_IDLE = {
				{
				PROBABILITY = 125,
				soundFile = "Languages/voicepacks/coreworkerD/alert_to_idle_al_1_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 125,
				soundFile = "Languages/voicepacks/coreworkerD/alert_to_idle_al_2_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 125,
				soundFile = "Languages/voicepacks/coreworkerD/alert_to_idle_al_3_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 125,
				soundFile = "Languages/voicepacks/coreworkerD/alert_to_idle_al_4_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 125,
				soundFile = "Languages/voicepacks/coreworkerD/alert_to_idle_al_5_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 125,
				soundFile = "Languages/voicepacks/coreworkerD/alert_to_idle_al_6_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 125,
				soundFile = "Languages/voicepacks/coreworkerD/alert_to_idle_al_7_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 125,
				soundFile = "Languages/voicepacks/coreworkerD/alert_to_idle_al_8_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
			},
			
	INTERESTED_TO_IDLE_GROUP = {
				{
				PROBABILITY = 200,
				soundFile = "Languages/voicepacks/coreworkerD/alert_to_idle_gp_1_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 200,
				soundFile = "Languages/voicepacks/coreworkerD/alert_to_idle_gp_2_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 200,
				soundFile = "Languages/voicepacks/coreworkerD/alert_to_idle_gp_3_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 200,
				soundFile = "Languages/voicepacks/coreworkerD/alert_to_idle_gp_4_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 200,
				soundFile = "Languages/voicepacks/coreworkerD/alert_to_idle_gp_5_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
			},
			
	THREATEN = {
				{
				PROBABILITY = 66,
				soundFile = "Languages/voicepacks/coreworkerD/combat_alone_1_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 66,
				soundFile = "Languages/voicepacks/coreworkerD/combat_alone_2_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 66,
				soundFile = "Languages/voicepacks/coreworkerD/combat_alone_3_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 66,
				soundFile = "Languages/voicepacks/coreworkerD/combat_alone_4_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 66,
				soundFile = "Languages/voicepacks/coreworkerD/combat_alone_5_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 66,
				soundFile = "Languages/voicepacks/coreworkerD/combat_alone_6_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 66,
				soundFile = "Languages/voicepacks/coreworkerD/combat_alone_7_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 66,
				soundFile = "Languages/voicepacks/coreworkerD/combat_alone_8_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 66,
				soundFile = "Languages/voicepacks/coreworkerD/combat_alone_9_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 66,
				soundFile = "Languages/voicepacks/coreworkerD/combat_alone_10_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 66,
				soundFile = "Languages/voicepacks/coreworkerD/combat_alone_11_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 66,
				soundFile = "Languages/voicepacks/coreworkerD/combat_alone_12_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 66,
				soundFile = "Languages/voicepacks/coreworkerD/combat_alone_13_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 66,
				soundFile = "Languages/voicepacks/coreworkerD/combat_alone_14_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 66,
				soundFile = "Languages/voicepacks/coreworkerD/combat_alone_15_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
			},
			
	THREATEN_GROUP = {
				{
				PROBABILITY = 142,
				soundFile = "Languages/voicepacks/coreworkerD/combat_group_1_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 142,
				soundFile = "Languages/voicepacks/coreworkerD/combat_group_2_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 142,
				soundFile = "Languages/voicepacks/coreworkerD/combat_group_3_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 142,
				soundFile = "Languages/voicepacks/coreworkerD/combat_group_4_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 142,
				soundFile = "Languages/voicepacks/coreworkerD/combat_group_5_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 142,
				soundFile = "Languages/voicepacks/coreworkerD/combat_group_6_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 142,
				soundFile = "Languages/voicepacks/coreworkerD/combat_group_7_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
			},
			
	ENEMY_TARGET_LOST = {
				{
				PROBABILITY = 125,
				soundFile = "Languages/voicepacks/coreworkerD/combat_to_alert_al_1_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 125,
				soundFile = "Languages/voicepacks/coreworkerD/combat_to_alert_al_2_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 125,
				soundFile = "Languages/voicepacks/coreworkerD/combat_to_alert_al_3_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 125,
				soundFile = "Languages/voicepacks/coreworkerD/combat_to_alert_al_4_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 125,
				soundFile = "Languages/voicepacks/coreworkerD/combat_to_alert_al_5_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 125,
				soundFile = "Languages/voicepacks/coreworkerD/combat_to_alert_al_6_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 125,
				soundFile = "Languages/voicepacks/coreworkerD/combat_to_alert_al_7_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 125,
				soundFile = "Languages/voicepacks/coreworkerD/combat_to_alert_al_8_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
			},
			
	ENEMY_TARGET_LOST_GROUP = {
				{
				PROBABILITY = 166,
				soundFile = "Languages/voicepacks/coreworkerD/combat_to_alert_gp_1_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 166,
				soundFile = "Languages/voicepacks/coreworkerD/combat_to_alert_gp_2_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 166,
				soundFile = "Languages/voicepacks/coreworkerD/combat_to_alert_gp_3_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 166,
				soundFile = "Languages/voicepacks/coreworkerD/combat_to_alert_gp_4_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 166,
				soundFile = "Languages/voicepacks/coreworkerD/combat_to_alert_gp_5_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 166,
				soundFile = "Languages/voicepacks/coreworkerD/combat_to_alert_gp_6_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
			},
						
	LO_SPLIT_LEFT = {
				{
				PROBABILITY = 333,
				soundFile = "Languages/voicepacks/coreworkerD/comm_flank_left_1_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 333,
				soundFile = "Languages/voicepacks/coreworkerD/comm_flank_left_2_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 333,
				soundFile = "Languages/voicepacks/coreworkerD/comm_flank_left_3_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
			},
			
	LO_SPLIT_RIGHT = {
				{
				PROBABILITY = 333,
				soundFile = "Languages/voicepacks/coreworkerD/comm_flank_right_1_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 333,
				soundFile = "Languages/voicepacks/coreworkerD/comm_flank_right_2_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 333,
				soundFile = "Languages/voicepacks/coreworkerD/comm_flank_right_3_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
			},
			
	RETREATING_NOW = {
				{
				PROBABILITY = 333,
				soundFile = "Languages/voicepacks/coreworkerD/comm_retreat_1_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 333,
				soundFile = "Languages/voicepacks/coreworkerD/comm_retreat_2_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 333,
				soundFile = "Languages/voicepacks/coreworkerD/comm_retreat_3_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
			},
					
	PROVIDING_COVER = {
				{
				PROBABILITY = 333,
				soundFile = "Languages/voicepacks/coreworkerD/comm_suppress_1_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 333,
				soundFile = "Languages/voicepacks/coreworkerD/comm_suppress_2_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 333,
				soundFile = "Languages/voicepacks/coreworkerD/comm_suppress_3_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
			},
			
	--filippo: this readability signal is not yet used by the AI
	ROOM_DARK = {
				--??
				--there are the wavs, but not the readability signal.
				{
				PROBABILITY = 200,
				soundFile = "Languages/voicepacks/coreworkerD/darkness_1_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 200,
				soundFile = "Languages/voicepacks/coreworkerD/darkness_2_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 200,
				soundFile = "Languages/voicepacks/coreworkerD/darkness_3_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 200,
				soundFile = "Languages/voicepacks/coreworkerD/darkness_4_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 200,
				soundFile = "Languages/voicepacks/coreworkerD/darkness_5_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
			},
						
	FLASHBANG_GRENADE_EFFECT = {
				{
				PROBABILITY = 142,
				soundFile = "Languages/voicepacks/coreworkerD/flashbang_1_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 142,
				soundFile = "Languages/voicepacks/coreworkerD/flashbang_2_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 142,
				soundFile = "Languages/voicepacks/coreworkerD/flashbang_3_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 142,
				soundFile = "Languages/voicepacks/coreworkerD/flaslight_face_1_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 142,
				soundFile = "Languages/voicepacks/coreworkerD/flaslight_face_2_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 142,
				soundFile = "Languages/voicepacks/coreworkerD/flaslight_face_3_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 142,
				soundFile = "Languages/voicepacks/coreworkerD/flaslight_face_4_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
			},
			
	--receiving and throwing grenade?
	FIRE_IN_THE_HOLE = {
				{
				PROBABILITY = 250,
				soundFile = "Languages/voicepacks/coreworkerD/grenade_1_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 250,
				soundFile = "Languages/voicepacks/coreworkerD/grenade_2_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 250,
				soundFile = "Languages/voicepacks/coreworkerD/grenade_3_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 250,
				soundFile = "Languages/voicepacks/coreworkerD/grenade_4_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
			},
			
	IDLE_TO_THREATENED = {
				{
				PROBABILITY = 142,
				soundFile = "Languages/voicepacks/coreworkerD/idle_to_alert_al_2_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 142,
				soundFile = "Languages/voicepacks/coreworkerD/idle_to_alert_al_3_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 142,
				soundFile = "Languages/voicepacks/coreworkerD/idle_to_alert_al_4_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 142,
				soundFile = "Languages/voicepacks/coreworkerD/idle_to_alert_al_5_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 142,
				soundFile = "Languages/voicepacks/coreworkerD/idle_to_alert_al_6_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 142,
				soundFile = "Languages/voicepacks/coreworkerD/idle_to_alert_al_7_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 142,
				soundFile = "Languages/voicepacks/coreworkerD/idle_to_alert_al_8_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
			},
			
	IDLE_TO_THREATENED_GROUP = {
				{
				PROBABILITY = 333,
				soundFile = "Languages/voicepacks/coreworkerD/idle_to_alert_gp_1_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 333,
				soundFile = "Languages/voicepacks/coreworkerD/idle_to_alert_gp_2_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 333,
				soundFile = "Languages/voicepacks/coreworkerD/idle_to_alert_gp_3_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
			},
			
	IDLE_TO_INTERESTED = {
				{
				PROBABILITY = 90,
				soundFile = "Languages/voicepacks/coreworkerD/idle_alert_hear_al_1_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 90,
				soundFile = "Languages/voicepacks/coreworkerD/idle_alert_hear_al_2_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 90,
				soundFile = "Languages/voicepacks/coreworkerD/idle_alert_hear_al_3_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 90,
				soundFile = "Languages/voicepacks/coreworkerD/idle_alert_hear_al_4_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 90,
				soundFile = "Languages/voicepacks/coreworkerD/idle_alert_hear_al_5_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 90,
				soundFile = "Languages/voicepacks/coreworkerD/idle_alert_hear_al_6_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 90,
				soundFile = "Languages/voicepacks/coreworkerD/idle_alert_seen_al_1_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 90,
				soundFile = "Languages/voicepacks/coreworkerD/idle_alert_seen_al_2_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 90,
				soundFile = "Languages/voicepacks/coreworkerD/idle_alert_seen_al_3_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 90,
				soundFile = "Languages/voicepacks/coreworkerD/idle_alert_seen_al_4_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 90,
				soundFile = "Languages/voicepacks/coreworkerD/idle_alert_seen_al_5_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
			},
			
	IDLE_TO_INTERESTED_GROUP = {
				{
				PROBABILITY = 166,
				soundFile = "Languages/voicepacks/coreworkerD/idle_alert_hear_gp_1_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 166,
				soundFile = "Languages/voicepacks/coreworkerD/idle_alert_hear_gp_2_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 166,
				soundFile = "Languages/voicepacks/coreworkerD/idle_alert_hear_gp_3_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 166,
				soundFile = "Languages/voicepacks/coreworkerD/idle_alert_seen_gp_1_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 166,
				soundFile = "Languages/voicepacks/coreworkerD/idle_alert_seen_gp_2_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 166,
				soundFile = "Languages/voicepacks/coreworkerD/idle_alert_seen_gp_3_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
			},
			
	RANDOM_IDLE_SOUND = {
				{
				PROBABILITY = 1000,
				soundFile = "Languages/voicepacks/coreworkerD/idle_alone_sniffle_1_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
			},
			
	RELOADING = { --or lowammo?
				{
				PROBABILITY = 166,
				soundFile = "Languages/voicepacks/coreworkerD/low_ammo_1_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 166,
				soundFile = "Languages/voicepacks/coreworkerD/low_ammo_2_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 166,
				soundFile = "Languages/voicepacks/coreworkerD/low_ammo_3_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 166,
				soundFile = "Languages/voicepacks/coreworkerD/reloaded_1_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 166,
				soundFile = "Languages/voicepacks/coreworkerD/reloaded_2_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 166,
				soundFile = "Languages/voicepacks/coreworkerD/reloaded_3_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
			},
	
	FRIEND_DEATH = {
				{
				PROBABILITY = 333,
				soundFile = "Languages/voicepacks/coreworkerD/mandown_alone_1_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 333,
				soundFile = "Languages/voicepacks/coreworkerD/mandown_alone_2_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 333,
				soundFile = "Languages/voicepacks/coreworkerD/mandown_alone_3_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
			},
			
	FRIEND_DEATH_GROUP = {
				{
				PROBABILITY = 250,
				soundFile = "Languages/voicepacks/coreworkerD/mandown_group_1_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 250,
				soundFile = "Languages/voicepacks/coreworkerD/mandown_group_2_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 250,
				soundFile = "Languages/voicepacks/coreworkerD/mandown_group_3_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 250,
				soundFile = "Languages/voicepacks/coreworkerD/mandown_group_4_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
			},
			
	--filippo: this readability signal is not yet used by the AI			
	GETTING_SHOT_AT = {
				--??
				{
				PROBABILITY = 166,
				soundFile = "Languages/voicepacks/coreworkerD/receive_fire_alone_1_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 166,
				soundFile = "Languages/voicepacks/coreworkerD/receive_fire_alone_2_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 166,
				soundFile = "Languages/voicepacks/coreworkerD/receive_fire_alone_3_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 166,
				soundFile = "Languages/voicepacks/coreworkerD/receive_fire_alone_4_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 166,
				soundFile = "Languages/voicepacks/coreworkerD/receive_fire_alone_5_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 166,
				soundFile = "Languages/voicepacks/coreworkerD/receive_fire_alone_6_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
			},
			
	--filippo: this readability signal is not yet used by the AI
	GETTING_SHOT_AT_GROUP = {
				--??
				{
				PROBABILITY = 200,
				soundFile = "Languages/voicepacks/coreworkerD/receive_fire_group_1_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 200,
				soundFile = "Languages/voicepacks/coreworkerD/receive_fire_group_2_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 200,
				soundFile = "Languages/voicepacks/coreworkerD/receive_fire_group_3_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 200,
				soundFile = "Languages/voicepacks/coreworkerD/receive_fire_group_4_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 200,
				soundFile = "Languages/voicepacks/coreworkerD/receive_fire_group_5_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
			},
			
	GET_REINFORCEMENTS = {
				{
				PROBABILITY = 100,
				soundFile = "Languages/voicepacks/coreworkerD/reinforce_go_get_1_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 100,
				soundFile = "Languages/voicepacks/coreworkerD/reinforce_go_get_2_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 100,
				soundFile = "Languages/voicepacks/coreworkerD/reinforce_go_get_3_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 100,
				soundFile = "Languages/voicepacks/coreworkerD/reinforce_go_get_4_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 100,
				soundFile = "Languages/voicepacks/coreworkerD/reinforce_go_get_5_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				--
				{
				PROBABILITY = 100,
				soundFile = "Languages/voicepacks/coreworkerD/reinforce_tell_get_1_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 100,
				soundFile = "Languages/voicepacks/coreworkerD/reinforce_tell_get_2_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 100,
				soundFile = "Languages/voicepacks/coreworkerD/reinforce_tell_get_3_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 100,
				soundFile = "Languages/voicepacks/coreworkerD/reinforce_tell_get_4_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 100,
				soundFile = "Languages/voicepacks/coreworkerD/reinforce_tell_get_5_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
			},
			
	--filippo: this readability signal is not yet used by the AI
	CALL_REINFORCEMENTS = {
				--??
				{
				PROBABILITY = 200,
				soundFile = "Languages/voicepacks/coreworkerD/reinforce_talk_to_1_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 200,
				soundFile = "Languages/voicepacks/coreworkerD/reinforce_talk_to_2_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 200,
				soundFile = "Languages/voicepacks/coreworkerD/reinforce_talk_to_3_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 200,
				soundFile = "Languages/voicepacks/coreworkerD/reinforce_talk_to_4_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 200,
				soundFile = "Languages/voicepacks/coreworkerD/reinforce_talk_to_5_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
			},
						
	AI_AGGRESSIVE = {
				{
				PROBABILITY = 47,
				soundFile = "Languages/voicepacks/coreworkerD/yell_attention_1_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 47,
				soundFile = "Languages/voicepacks/coreworkerD/yell_attention_2_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 47,
				soundFile = "Languages/voicepacks/coreworkerD/yell_attention_3_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 47,
				soundFile = "Languages/voicepacks/coreworkerD/yell_attention_4_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 47,
				soundFile = "Languages/voicepacks/coreworkerD/yell_attention_5_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				
				--
				{
				PROBABILITY = 47,
				soundFile = "Languages/voicepacks/coreworkerD/yell_fear_1_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 47,
				soundFile = "Languages/voicepacks/coreworkerD/yell_fear_2_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 47,
				soundFile = "Languages/voicepacks/coreworkerD/yell_fear_3_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 47,
				soundFile = "Languages/voicepacks/coreworkerD/yell_fear_4_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 47,
				soundFile = "Languages/voicepacks/coreworkerD/yell_fear_5_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 47,
				soundFile = "Languages/voicepacks/coreworkerD/yell_fear_6_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 47,
				soundFile = "Languages/voicepacks/coreworkerD/yell_fear_7_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 47,
				soundFile = "Languages/voicepacks/coreworkerD/yell_fear_8_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				
				--
				{
				PROBABILITY = 47,
				soundFile = "Languages/voicepacks/coreworkerD/yell_macho_1_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 47,
				soundFile = "Languages/voicepacks/coreworkerD/yell_macho_2_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 47,
				soundFile = "Languages/voicepacks/coreworkerD/yell_macho_3_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 47,
				soundFile = "Languages/voicepacks/coreworkerD/yell_macho_4_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 47,
				soundFile = "Languages/voicepacks/coreworkerD/yell_macho_5_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 47,
				soundFile = "Languages/voicepacks/coreworkerD/yell_macho_6_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 47,
				soundFile = "Languages/voicepacks/coreworkerD/yell_macho_7_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
				{
				PROBABILITY = 47,
				soundFile = "Languages/voicepacks/coreworkerD/yell_macho_8_VoiceD.wav",
				Volume = 175,
				min = 12,
				max = 200,
				sound_unscalable = 0,
				},
			},
}