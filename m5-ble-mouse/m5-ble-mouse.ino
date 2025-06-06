#include <M5Unified.h>
#include <BleMouse.h>

// Device Nameを好きな名称に設定してください
BleMouse bleMouse("Device Name");

bool connected = false;

// 移動平均用の変数
float filteredAx = 0.0;
float filteredAy = 0.0;
const float alpha = 0.2; // 移動平均の係数

// ノイズしきい値
const float threshold = 0.05;

void setup()
{
  M5.begin();
  M5.Power.begin();

  Serial.begin(115200);
  Serial.println("Starting BLE work!");
  bleMouse.begin();

  M5.Display.setRotation(0);
  M5.Display.fillScreen(BLACK);
  M5.Display.setTextColor(WHITE);
  M5.Display.setTextSize(1);
  M5.Display.setCursor(0, 0);
  M5.Display.println("BLE Mouse Example");
  M5.Display.println("Connecting...");
}

void loop()
{
  if (!bleMouse.isConnected())
    return;
  else if (!connected)
  {
    M5.Display.fillScreen(BLACK);
    M5.Display.setCursor(0, 0);
    M5.Display.println("BLE Mouse Example");
    M5.Display.println("Connected!");
    connected = true;
  }

  M5.update();

  float ax, ay, az;
  M5.Imu.getAccelData(&ax, &ay, &az);

  // 移動平均でスムージング
  filteredAx = alpha * ax + (1 - alpha) * filteredAx;
  filteredAy = alpha * ay + (1 - alpha) * filteredAy;

  // ノイズ除去
  float dx = fabs(filteredAx) > threshold ? filteredAx : 0.0;
  float dy = fabs(filteredAy) > threshold ? filteredAy : 0.0;

  // マウス移動に変換
  int8_t mouseX = static_cast<int8_t>(dx * -15);
  int8_t mouseY = static_cast<int8_t>(dy * -15);

  // 移動があるときのみ送信
  if (mouseX != 0 || mouseY != 0)
  {
    bleMouse.move(mouseX, mouseY, 0);
    Serial.printf("Mouse Move: X=%d, Y=%d\n", mouseX, mouseY);
  }

  // ボタン処理
  if (M5.BtnA.wasPressed())
  {
    bleMouse.click(MOUSE_LEFT);
  }
  if (M5.BtnB.wasPressed())
  {
    bleMouse.click(MOUSE_RIGHT);
  }

  delay(20);
}
