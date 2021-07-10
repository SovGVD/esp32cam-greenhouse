// TODO make is as one of the sensor???


/*********
 * Camera code inspired by and copy-pasted from:

  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-cam-take-photo-save-microsd-card
  
  IMPORTANT!!! 
   - Select Board "AI Thinker ESP32-CAM"
   - GPIO 0 must be connected to GND to upload a sketch
   - After connecting GPIO 0 to GND, press the ESP32-CAM on-board RESET button to put your board in flashing mode
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

#ifdef CAMERA_ENABLED
uint8_t cameraPicCount = 0;

void setupCamera(fs::FS &fs)
{
  cliSerial->println("Try ESP32CAM");

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG; 
  
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA; // FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  cliSerial->println("ESP32CAM initialisation...");
  
  // Init Camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    cliSerial->printf("Camera init failed with error 0x%x", err);
    return;
  }

  cliSerial->println("ESP32CAM cam settings...");
  sensor_t * s = esp_camera_sensor_get();
  s->set_brightness(s, 0);     // -2 to 2
  s->set_contrast(s, 0);       // -2 to 2
  s->set_saturation(s, 0);     // -2 to 2
  s->set_special_effect(s, 0); // 0 to 6 (0 - No Effect, 1 - Negative, 2 - Grayscale, 3 - Red Tint, 4 - Green Tint, 5 - Blue Tint, 6 - Sepia)
  s->set_whitebal(s, 1);       // 0 = disable , 1 = enable
  s->set_awb_gain(s, 1);       // 0 = disable , 1 = enable
  s->set_wb_mode(s, 0);        // 0 to 4 - if awb_gain enabled (0 - Auto, 1 - Sunny, 2 - Cloudy, 3 - Office, 4 - Home)
  s->set_exposure_ctrl(s, 1);  // 0 = disable , 1 = enable
  s->set_aec2(s, 1);           // 0 = disable , 1 = enable
  s->set_ae_level(s, 0);       // -2 to 2
  s->set_aec_value(s, 300);    // 0 to 1200
  s->set_gain_ctrl(s, 1);      // 0 = disable , 1 = enable
  s->set_agc_gain(s, 0);       // 0 to 30
  s->set_gainceiling(s, (gainceiling_t)0);  // 0 to 6
  s->set_bpc(s, 0);            // 0 = disable , 1 = enable
  s->set_wpc(s, 1);            // 0 = disable , 1 = enable
  s->set_raw_gma(s, 1);        // 0 = disable , 1 = enable
  s->set_lenc(s, 1);           // 0 = disable , 1 = enable
  s->set_hmirror(s, 0);        // 0 = disable , 1 = enable
  s->set_vflip(s, 0);          // 0 = disable , 1 = enable
  s->set_dcw(s, 1);            // 0 = disable , 1 = enable
  s->set_colorbar(s, 0);       // 0 = disable , 1 = enable


  // setup folder
  if (isStorageAvailable) {
    cliSerial->println("ESP32CAM check or create folder");
    String path = "/" + String(PIC_FOLDER);
    if (!fs.exists(path)) {
      fs.mkdir(path);
    }
  }

  cliSerial->println("ESP32CAM init finished");
  appendSensor(0, SENSOR_ESP32CAM, 0x0, VALUE_TYPE_JPEG);
}

void cameraSaveFolder(fs::FS &fs)
{
  if (!isStorageAvailable) {
    return;
  }

  String path = "/" + String(PIC_FOLDER) + "/" + getNameYM();
  if (!fs.exists(path)) {
    fs.mkdir(path);
  }
  
}

void askSensor_ESP32CAM(uint8_t idx, bool forceAction)
{
  if ((cameraPicCount > 3 && isTimeSynced) || (forceAction && !sensors[idx].isDataReady)) {
    if (takePhoto(SD_MMC)) {
      sensors[idx].value = 1;
      sensors[idx].isDataReady = true;
    } else {
      sensors[idx].value = 0;
      sensors[idx].isDataReady = true;
      sensors[idx].isDataFailed = true;
    }

    return;
  }

  cameraPicCount++;
}


bool takePhoto(fs::FS &fs)
{
  if (!isStorageAvailable) {
    return false;
  }

  cameraSaveFolder(fs);

  // Take Picture with Camera
  fb = esp_camera_fb_get();
 
  if(!fb) {
    cliSerial->println("Camera capture failed");
    return false;
  }

  // Path where new picture will be saved in SD Card
  String path = "/" + String(PIC_FOLDER) + "/" + getNameYM() + "/" + getNameYMDHIS() + "-" + String(bootCounter) + ".jpg";

  cliSerial->printf("Picture file name: %s\n", path.c_str());
  
  File file = fs.open(path.c_str(), FILE_WRITE);
  if(!file){
    cliSerial->println("Failed to open file in writing mode");
  } else {
    file.write(fb->buf, fb->len); // payload (image), payload length
    cliSerial->printf("Saved file to path: %s\n", path.c_str());
  }
  file.close();
  
  esp_camera_fb_return(fb);
  return !!file;
}
#endif
