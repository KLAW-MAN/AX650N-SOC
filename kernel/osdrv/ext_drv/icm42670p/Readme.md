1.To use this module, you need to configure the DTS file, default use SPI0 port, chip select CS0, DTS configuration is as follows:

&spi0 {
	status = "okay";
	rx-sample-delay-ns = <40>;

	icm42670_spi: icm42670_spi@0 {
		status = "okay";
		compatible = "axera,icm42670_spi";
		reg = <0>;    /*SPI channel 0*/
		spi-max-frequency = <10000000>;

		accel-param = <100>, <16>;
		gyro-param = <100>, <2000>;

		high-resolution-enable;
	};
};

accel-param[0]: Used to set Acceleromete output frame rate in Hz, support frame rate: 10, 25, 50, 100, 200, 400, 800, 1600.
                If the setting is not in the support range, the default is 1600Hz.
accel-param[1]: Used to set the Acceleromete range in G. Supported ranges: 2, 4, 8, 16. If the setting is not in the supported
                range, 16G is used by default.
gyro-param[0]: Used to set the Gyro output frame rate in Hz, support frame rate: 10, 25, 50, 100, 200, 400, 800, 1600. If the
               setting is not in the support range, the default is 1600Hz.
gyro-param[1]: Used to set Gyro ODR in dps, support range: 250, 500, 1000, 2000. if the setting is not in the support range,
               the default is 2000dps.
high-resolution-enable: Configure this attribute to enable high-resolution and remove it if high-resolution is disabled.


2.The data acquired by the user consists of:
	pdata->data.acc_data[index].x = ((uint32_t)(event->accel_high_res[0]) << 16 | (uint32_t)event->accel[0]);
	pdata->data.acc_data[index].y = ((uint32_t)(event->accel_high_res[1]) << 16 | (uint32_t)event->accel[1]);
	pdata->data.acc_data[index].z = ((uint32_t)(event->accel_high_res[2]) << 16 | (uint32_t)event->accel[2]);
	pdata->data.gyro_data[index].x = ((uint32_t)(event->gyro_high_res[0]) << 16 | (uint32_t)event->gyro[0]);
	pdata->data.gyro_data[index].y = ((uint32_t)(event->gyro_high_res[1]) << 16 | (uint32_t)event->gyro[1]);
	pdata->data.gyro_data[index].z = ((uint32_t)(event->gyro_high_res[2]) << 16 | (uint32_t)event->gyro[2]);
	pdata->data.temperature[index] = (uint32_t)event->temperature;
	pdata->data.timestamp[index] = (uint32_t)event->timestamp_fsync;