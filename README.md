# ⚡ PowerWatch

**PowerWatch** is an ESP8266 (Wemos D1 Mini) based power-restoration notifier.  
When electricity returns, the device boots, connects to Wi-Fi and sends a Telegram notification.  
At the same time RGB LEDs and two buzzers run a visible/audible alarm so you won't miss the power return. A hardware OFF switch lets you silence the alarm without unplugging the board.

---

## 🔧 Features
- Sends a **Telegram** message when power is restored
- Dual **active buzzers** for loud audible alert
- **RGB LED** color effects for visual notification
- Automatic Wi-Fi connection with retry/backoff
- Hardware **OFF** switch to manually disable the alarm sequence
- Project files include Fritzing schematic and Gerber files

---

## ⚙️ How it works
1. Power returns → PowerWatch (Wemos D1 mini) powers on.  
2. The ESP8266 connects to the configured Wi-Fi network.  
3. Once connected, it sends a Telegram message to the configured bot/chat.  
4. Buzzers and RGB LEDs also start an alarm sequence to wake anyone nearby.  
5. Use the hardware OFF switch to stop the alarm signals after acknowledging.

This is handy if you’re away from home (get a Telegram ping) or sleeping (buzzers + LED wake you).

---

## 🔌 Quickstart (flash & run)
1. Open `src/main.ino` in Arduino IDE.  
2. Board: **Wemos D1 R2 & mini** (select appropriate ESP8266 board).  
3. Install `ESP8266` core and ensure `ESP8266WiFi.h` is available.  
4. Edit configuration values (see *Configuration* below).  
5. Upload to the Wemos D1 mini.  
6. Plug the board into a stable USB power supply. On power return the device will act.

---

## ⚙️ Configuration (what to change)
Open `src/PowerWatch.ino` and set your Wi-Fi and Telegram values:

```cpp
const char* ssid     = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

String BOT_TOKEN = "YOUR_BOT_TOKEN";
String CHAT_ID   = "YOUR_CHAT_ID";


String NOTIFY_MESSAGE = "⚡ Power is back!";   // set your custom message
```

---

## 📲 Telegram setup
**Get a bot token (BOT_TOKEN):**
1. In Telegram open [@BotFather](https://t.me/BotFather).  
2. Send `/newbot` and follow prompts to create a bot.  
3. Copy the token BotFather returns (looks like `123456789:ABC...`).

**Get your chat id (CHAT_ID):**
- For a private chat: start a conversation with a helper bot like [@userinfobot](https://t.me/userinfobot) or implement a simple `/start` handler in your bot to log the `chat.id`.  
- For a group: add your bot to the group and use a helper bot or read messages via the bot to get the group id (group IDs often start with `-100...`).

---

## 🧩 Hardware

| Qty | Part | Notes |
|-----:|------|-------|
| 1 | Wemos D1 Mini (ESP8266) | main controller |
| 2 | Piezo speaker (active buzzer) | pins → D5, D8 |
| 1 | RGB LED (common cathode) | pins → D1 (R), D2 (G), D7 (B) |
| 3 | 220 Ω resistors | current limit for R/G/B |
| 1 | 330 kΩ resistor | pull-down for switch (per your Fritzing) |
| 1 | ss12d00g3 switch | OFF control |

Files included in `hardware/`:
- `scheme.fzz` — original Fritzing source  
- `gerber/` — Gerber export folder if you made a PCB  

---

## 🖼 Photos & Schematics


## ⚠️ Wiring notes & common pitfalls
- Use **common cathode RGB** wiring if your RGB is common cathode (your Fritzing BOM says common cathode). Code uses inverted PWM (setColor uses `255 - r` style). Verify LED type first.  
- Buzzers: use **active** buzzers (they take a DC signal and produce tone). If using passive piezos you’ll need `tone()` support (your code uses `tone()`; active buzzers may just need HIGH). Test on bench first.  
- D0 (GPIO16) may behave differently on ESP8266 regarding boot/pull-ups. Ensure your OFF switch wiring does not pull GPIO into a state that prevents boot. Consider `INPUT_PULLUP` and wiring the switch to GND.  
- If Telegram messages fail, check SSL and that `client.setInsecure()` is used (your sketch sets it) or add certificate checks as required.

---

## Troubleshooting
- **No Telegram message:** check Wi-Fi creds, BOT_TOKEN and CHAT_ID; check board has internet access; open Serial Monitor for debug.  
- **ESP won’t boot after switch wiring:** check switch wiring and pull resistors — make sure boot pins aren’t held in wrong state.  
- **RGB colors inverted / wrong:** confirm RGB is common-cathode vs common-anode and adjust `setColor()` accordingly.

---

## Acknowledgements
Built during frequent power outages and blackouts in 🇺🇦
