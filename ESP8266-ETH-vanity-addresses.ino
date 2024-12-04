#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <ESP_Mail_Client.h>
#include <uECC.h>
#include "src/Keccak.h"
#ifdef ESP8266
    extern "C" {
        #include "user_interface.h"
    }
#endif

// WiFi configuration
const char* ssid = "your_ssid";
const char* password = "your_password";

// Konfigurasi Email
#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465
#define AUTHOR_EMAIL "your_email_address@gmail.com"
#define AUTHOR_PASSWORD "your_google_app_password"
#define RECIPIENT_EMAIL "recipient_email_address@gmail.com"

// Target prefix
//example 0x0
const String TARGET_PREFIX = "0x0";

// Variabel global
unsigned long attempts = 0;
const struct uECC_Curve_t * curve = uECC_secp256k1();
uint8_t privateKey[32];
uint8_t publicKey[64];
uint8_t uncompressedPubKey[65];
uint8_t hash[32];
Keccak keccak;
SMTPSession smtp;

String toHexString(uint8_t *data, size_t length) {
    String hex = "";
    for (size_t i = 0; i < length; i++) {
        if (data[i] < 16) hex += "0";
        hex += String(data[i], HEX);
    }
    return hex;
}

void generatePrivateKey() {
    bool isValid;
    do {
        isValid = false;
        for(int i = 0; i < 32; i++) {
            #ifdef ESP32
                privateKey[i] = (uint8_t)esp_random();
            #else
                privateKey[i] = (uint8_t)os_random();
            #endif
        }
        
        // Simple validation to ensure the private key is not null
        for(int i = 0; i < 32; i++) {
            if(privateKey[i] != 0) {
                isValid = true;
                break;
            }
        }
    } while (!isValid || !uECC_compute_public_key(privateKey, publicKey, curve));
}

bool derivePublicKey() {
    if (!uECC_compute_public_key(privateKey, publicKey, curve)) {
        return false;
    }
    
   // Reformat the public key according to Ethereum standards
    uncompressedPubKey[0] = 0x04;  // Prefix for uncompressed key
    memcpy(uncompressedPubKey + 1, publicKey, 64);
    return true;
}

String deriveAddress() {
    if (!derivePublicKey()) {
        return "Error";
    }
    
    // Hash public key (skip first byte 0x04)
    keccak.reset();
    keccak.add(uncompressedPubKey + 1, 64);
    keccak.finalize(hash);
    
    // Format address
    String address = "0x";
    for(int i = 12; i < 32; i++) {
        uint8_t b = hash[i];
        address += (b >> 4) <= 9 ? (char)('0' + (b >> 4)) : (char)('a' + (b >> 4) - 10);
        address += (b & 0xF) <= 9 ? (char)('0' + (b & 0xF)) : (char)('a' + (b & 0xF) - 10);
    }
    return address;
}

void sendEmail(String privateKey, String address) {
    ESP_Mail_Session session;
    session.server.host_name = SMTP_HOST;
    session.server.port = SMTP_PORT;
    session.login.email = AUTHOR_EMAIL;
    session.login.password = AUTHOR_PASSWORD;
    
    SMTP_Message message;
    message.sender.name = "Wallet Generator";
    message.sender.email = AUTHOR_EMAIL;
    message.subject = "Wallet Found!";
    message.addRecipient("User", RECIPIENT_EMAIL);
    
    String content = "Private Key: " + privateKey + "\n";
    content += "Address: " + address + "\n";
    content += "Total Attempts: " + String(attempts);
    message.text.content = content.c_str();
    
    if (!smtp.connect(&session)) {
        Serial.println("SMTP Connection Failed!");
        return;
    }
    
    if (!MailClient.sendMail(&smtp, &message)) {
        Serial.println("Error sending email: " + smtp.errorReason());
    }
}

void printSystemInfo() {
    Serial.println("\n+------------------+------------------+");
    Serial.println("| Parameter        | Value            |");
    Serial.println("+------------------+------------------+");
    // Free Memory
    Serial.print("| Free Memory      | ");
    Serial.print(ESP.getFreeHeap());
    Serial.println(" bytes       |");
    // CPU Frequency  
    Serial.print("| CPU Frequency    | ");
    Serial.print(ESP.getCpuFreqMHz());
    Serial.println(" MHz          |");
    // Boot Mode
    Serial.print("| Flash Chip Speed | ");
    Serial.print(ESP.getFlashChipSpeed());
    Serial.println("              |");
    // Free Sketch Space
    Serial.print("| Free Sketch Space| ");
    Serial.print(ESP.getFreeSketchSpace());
    Serial.println(" bytes    |");
    // Chip ID
    Serial.print("| Chip ID          | ");
    Serial.print(ESP.getChipId(), HEX);
    Serial.println("           |");
    
    Serial.println("+------------------+------------------+");

    if(ESP.getFreeHeap()<4000){
      Serial.println("Low Memory, restarting...");
      ESP.restart();
    }
}

void setup() {
    Serial.begin(115200);
    
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println("\Connected..!");
    Serial.println("Start Find Address...");
    Serial.println("Target prefix: " + TARGET_PREFIX);
}

void loop() {
    generatePrivateKey();
    String privateKeyHex = "0x" + toHexString(privateKey, 32);
    String address = deriveAddress();
    
    Serial.print("Private Key: ");
    Serial.println(privateKeyHex);
    Serial.print("Address: ");
    Serial.println(address);
    
    attempts++;
    
    if (address.startsWith(TARGET_PREFIX)) {
        Serial.println("\nMatching address found!");
        sendEmail(privateKeyHex, address);
        delay(5000);
    }
    
    if(attempts % 1000 == 0) {
      Serial.print(attempts);
      Serial.println("th try");
    }
    printSystemInfo();
    yield();
}