#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include "config.h"
#include "display.h"

Adafruit_SH1106G display(128, 64, &Wire, -1);

static void draw_bars(int bars)
{
    int x = 110;
    int y = 10;

    for (int i = 0; i < 4; i++)
    {
        int h = 2 + i * 2;

        if (i < bars)
            display.fillRect(x + i * 3, y - h, 2, h, SH110X_WHITE);
        else
            display.drawRect(x + i * 3, y - h, 2, h, SH110X_WHITE);
    }
}

static void draw_disconnect_mark()
{
    display.drawLine(110, 2, 118, 10, SH110X_WHITE);
    display.drawLine(118, 2, 110, 10, SH110X_WHITE);
}

static void display_wifi_icon(int bars, bool connected)
{
    if (connected)
        draw_bars(bars);
    else
        draw_disconnect_mark();
}

static void draw_text(String text, int x, int y, int maxWidth)
{
    int cursorX = x;
    int cursorY = y;
    String word = "";

    for (int i = 0; i < text.length(); i++)
    {
        char c = text[i];

        if (c == ' ' || i == text.length() - 1)
        {
            if (i == text.length() - 1) word += c;

            int wordWidth = word.length() * 6;

            if (cursorX + wordWidth > maxWidth)
            {
                cursorX = x;
                cursorY += 10;
            }

            display.setCursor(cursorX, cursorY);
            display.print(word);

            cursorX += wordWidth + 6;
            word = "";
        }
        else
        {
            word += c;
        }
    }
}

void display_init()
{
    Wire.begin(OLED_SDA, OLED_SCL);

    if (!display.begin(0x3C, true))
    {
        while (1);
    }

    display.clearDisplay();
    display.setTextColor(SH110X_WHITE);
    display.display();
}

// -------- MAIN --------
void display_passenger(String name, String flight, String seat, String gate, int bars, bool connected)
{
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println(name);
    display.setCursor(0, 12);
    display.println("Flt: " + flight);
    display.setCursor(0, 24);
    display.println("Seat: " + seat);
    display.setCursor(0, 36);
    display.println("Gate: " + gate);
    display_wifi_icon(bars, connected);
    display.display();
}

void display_notification(String title, String text, int bars, bool connected)
{
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println(title);
    draw_text(text, 0, 14, 120);
    display_wifi_icon(bars, connected);
    display.display();
}

// -------- WIFI --------
void display_wifi_connecting()
{
    display.clearDisplay();
    display.setCursor(10, 28);
    display.println("Connecting WiFi...");
    display.display();
}

void display_wifi_connected()
{
    display.clearDisplay();
    display.setCursor(10, 28);
    display.println("WiFi Connected");
    display.display();
}

void display_wifi_disconnected()
{
    display.clearDisplay();
    display.setCursor(20, 28);
    display.println("No WiFi");
    display.display();
}

void display_reconnecting()
{
    display.clearDisplay();
    display.setCursor(10, 28);
    display.println("Reconnecting...");
    display.display();
}

// -------- SERVER --------
void display_server_connecting()
{
    display.clearDisplay();
    display.setCursor(5, 28);
    display.println("Connecting Server...");
    display.display();
}

void display_registering()
{
    display.clearDisplay();
    display.setCursor(25, 28);
    display.println("Registering...");
    display.display();
}

void display_server_error(int bars)
{
    display.clearDisplay();
    display.setCursor(15, 28);
    display.println("Server Error");
    display_wifi_icon(bars, false);
    display.display();
}

void display_server_retry()
{
    display.clearDisplay();
    display.setCursor(20, 28);
    display.println("Retrying...");
    display.display();
}

// -------- POWER --------
void display_sleep()
{
    display.clearDisplay();
    display.display();
}