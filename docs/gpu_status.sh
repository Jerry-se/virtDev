#!/bin/bash

FUNCTION=$1
GPU_ID=$2

function gpu_usages () {
	usages=$(nvidia-smi -q -i ${GPU_ID} -d UTILIZATION|egrep -A 4 "Utilization$"|grep Gpu|awk '{print $3}')
	echo $usages
}
function gpu_numbers () {
	total_numbers=$(nvidia-smi -L|wc -l)
	echo $total_numbers
}
function gpu_fans () {
	fan_speed=$(nvidia-smi -q -i ${GPU_ID}|grep -i fan|awk '{print $4}')
	echo $fan_speed
}
case ${FUNCTION} in
	"get_numbers")
		gpu_numbers
		;;
	"get_usages")
		gpu_usages
		;;
	"get_fanspeed")
		gpu_fans
		;;
esac

