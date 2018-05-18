#include "ST7565_homephone.h"
#include "bmps.h"
#include "mario.h"

#ifdef __AVR__
#include <avr/io.h>
#include <avr/pgmspace.h>
#endif

#define melodyPin 8
#define egg_max 15
#define start A5
#define dieu_khien A0
#define loa 8
ST7565 lcd(3, 4, 5, 6, 7);

int song;
int melody_start[] = {
  NOTE_E7, NOTE_E7, 0, NOTE_E7,
  0, NOTE_C7, NOTE_E7, 0,
  NOTE_G7, 0, 0,  0,
  NOTE_G6, 0, 0, 0,

  NOTE_G6, NOTE_E7, NOTE_G7,
  NOTE_A7, 0, NOTE_F7, NOTE_G7,
  0, NOTE_E7, 0, NOTE_C7,
  NOTE_D7, NOTE_B6, 0, 0,

  NOTE_G6, NOTE_E7, NOTE_G7,
  NOTE_A7, 0, NOTE_F7, NOTE_G7,
  0, NOTE_E7, 0, NOTE_C7,
  NOTE_D7, NOTE_B6, 0, 0
};

int tempo_start[] = {
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  9, 9, 9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  9, 9, 9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
};

int melody_gameover[] = {
  0, NOTE_DS4, NOTE_CS4, NOTE_D4,
  NOTE_CS4, NOTE_DS4,
  NOTE_DS4, NOTE_GS3,
  NOTE_C4, NOTE_FS4, NOTE_F4, NOTE_E3, NOTE_AS4, NOTE_A4,
  NOTE_GS4, NOTE_DS4, NOTE_B3,
  0, 0, 0
};

int tempo_gameover[] = {
  6, 18, 18, 18,
  6, 6,
  6, 6,
  18, 18, 18, 18, 18, 18,
  10, 10, 10,
  3, 3, 3
};

const static unsigned char __attribute__ ((progmem)) ga[] = {
  0x20, 0xD8, 0x2C, 0x0C, 0x9E, 0x72, 0x42, 0xC0, 0x40, 0x40, 0x60, 0xFC, 0xCE, 0x3B, 0x7B, 0x01,
  0x00, 0x06, 0x09, 0x10, 0x30, 0x48, 0x44, 0x4E, 0x59, 0x71, 0x39, 0x1F, 0x0E, 0x00, 0x00, 0x00
};

const static unsigned char __attribute__ ((progmem)) trung[] = {
  0x1C, 0x22, 0x41, 0x41, 0x22, 0x1C,
};

const static unsigned char __attribute__ ((progmem)) nhan_vat[] = {
  0x60, 0x9C, 0x62, 0x01, 0x01, 0xC1, 0x26, 0x29, 0x29, 0x4B, 0x4E, 0x48, 0x70, 0x00, 0xC0, 0x80,
  0x80, 0x80, 0x80, 0x80, 0x80, 0xC0, 0x00, 0x00, 0x01, 0x01, 0x02, 0xE4, 0xFF, 0xFE, 0xFE, 0xFE,
  0xFC, 0x1C, 0x0E, 0x06, 0x1F, 0x21, 0x52, 0x44, 0x48, 0x52, 0x21, 0x1F, 0x00, 0x00, 0x64, 0x70,
  0x2F,
};

// mảng cho 2 đối tượng 1 gà(0) và egg_max  trứng(1,2...egg_max )
byte obj_ga_trung[1 + egg_max][5] = {0};
byte obj_nhan_vat[4] = {0};
byte cap = 1;
byte checkStart = 0;

byte Xmin, Ymin, Xmax, Ymax; //biên
int diem, mang;
byte timedelay;
byte speedEgg = 1;
byte speedChicken = 1;

void setup()   {
  lcd.ON();
  lcd.SET(23, 0, 0, 0, 4);
  pinMode(dieu_khien, INPUT_PULLUP);
  pinMode(start, INPUT_PULLUP);
  pinMode(loa, OUTPUT);//loa

  Xmin = 0;
  Xmax = 125;
  Ymin = 0;
  Ymax = 63;
  mang = 3;
  diem = 0;
  timedelay = 0;
  start_game();
}

//Giao dien truoc khi bat dau game
void start_game() {
  for (byte x = Xmin + 15; x < Xmax ; x += 20) {
    lcd.elip(x, Ymax - 10 , 5, 10, BLACK);
    lcd.display();
  }
  for (byte x = (Xmin + 15); x < Xmax; x += 20) {
    for (byte a = 5; a <= 15; a++) {
      lcd.elip(x, Ymax - 15 + a, a, 15 - a, BLACK);
      lcd.display();
      delay(50);
      lcd.elip(x, Ymax - 15 + a, a, 15 - a, WHITE);
      lcd.display();
    }
    for (int y = Ymax - 20; y >= (-10); y -= 4) {
      lcd.elip(x, y, 5, 14, BLACK);
      lcd.display();
      delay(10);
      lcd.elip(x, y, 5, 14, WHITE);
      lcd.display();
    }
  }
  lcd.clear();
  lcd.fillrect(Xmin + 30, Ymin + 10 , 100, 64, DELETE);
  lcd.rect(8, 22, 118, 11, BLACK);
  lcd.Asc_String( 10, 24, Asc("--GAME HUNG TRUNG--"), BLACK);
  lcd.rect(58, 54, 70, 11, BLACK);
  lcd.Asc_String( 60, 55, Asc("By 14T DUT"), BLACK);
  lcd.display();
  song = 1;
  sing(song);
  lap_vo_han();
  lcd.clear();
}

//===========================================================
////////////////DI CHUYỂN GÀ////////////////////

void di_chuyen_ga( ) {
  byte x_cu, y_cu, huong_cu;
  byte x_moi;
  byte a, b;
  x_cu = obj_ga_trung[0][1];
  y_cu = obj_ga_trung[0][2];
  huong_cu = obj_ga_trung[0][3];
  //lấy thông tin
  if (diem < 15) speedChicken = 1;
  else if ((diem % 15 == 0) && (speedChicken < 3)) {
    speedChicken += 1;
    lcd.Asc_String(20, 20 , Asc("Ga tang toc!"), BLACK);
    lcd.Asc_String(20, 30 , Asc("Da len cap!"), BLACK);
    lcd.display();
    delay(1000);
    lcd.fillrect(20, 20, 40, 30, DELETE);
    cap+=1;
  }
//  if (diem == 15) cap = 2;
//  else if (diem > 29) cap = 3;

  if (huong_cu == 1) { //gà quay về bên phải
    x_moi = x_cu + speedChicken;
    lcd.Plus_Bitmap(x_moi, y_cu, 16, 16, ga, 0, MIRROR, BLACK);
    // xoay bitmap (mirror)
  }

  if (huong_cu == 3) { //gà quay về bên trái
    x_moi = x_cu - speedChicken;
    lcd.Plus_Bitmap(x_moi, y_cu, 16, 16, ga, 0, NO_MIRROR, BLACK);
    // không xoay (non mirror)
  }

  //ghi lại thông tin
  obj_ga_trung[0][1] = x_moi;
  lcd.display();

}//vẽ gà

void xoa_ga( byte x, byte y) {
  lcd.fillrect(x, y, 18, 18, DELETE);//
  lcd.display();
}//xóa gà

void ga_di_va_de_trung() { // gà đi và đẻ trứng
  //////////// XỬ LÍ GÀ////////////
  //b1: khởi tạo đối tượng
  if (obj_ga_trung[0][0] == 0) {
    // nếu không tồn tại gà thì phải tạo gà
    // cấp thuộc tính cho đối tượng gà
    obj_ga_trung[0][1] = Xmin + 5; //hoành độ ban đầu
    obj_ga_trung[0][2] = Ymin; // tung độ ban đầu
    obj_ga_trung[0][3] = 1; //hướng sang bên phải
    obj_ga_trung[0][0] = 1; //cấp quyền tồn tại
  }
  //b2: gà đi và quay đầu
  //lấy thông tin;

  if (obj_ga_trung[0][1] <= Xmin) {
    // nếu gà đến biên trái thì phải quay đầu về bên phải
    obj_ga_trung[0][3] = 1;
    //ghi lại thông tin
  }

  if (obj_ga_trung[0][1] >= Xmax) {
    // nếu gà đến biên phải thì phải quay đầu về bên trái
    obj_ga_trung[0][3] = 3;
    //ghi lại thông tin
  }
  ///////////GÀ ĐANG ĐI/////////
  // lấy tt
  xoa_ga( obj_ga_trung[0][1], obj_ga_trung[0][2]);//xóa ảnh cũ
  di_chuyen_ga();// vẽ ảnh mới
}

//===============================================================
////////////////////TRỨNG RƠI//////////////////////
void ve_trung(byte x, byte y) {
  lcd.bitmap(x, y, 6, 7, trung, BLACK);
  lcd.display();
}

void xoa_trung( byte x, byte y) {
  //xóa = hình chữ nhật màu trắng
  lcd.fillrect(x, y, 7, 7, DELETE);
  lcd.display();
}

void di_chuyen_trung( byte huong_cu, byte i) {
  byte x_cu, y_cu, x_moi, y_moi, ton_tai, huong_moi;
  //lây tt
  x_cu = obj_ga_trung[i][1];
  y_cu = obj_ga_trung[i][2];
  ton_tai = obj_ga_trung[i][0];

  if (diem < 10) speedEgg = 1;
  else if ((diem % 10 == 0) && (speedEgg < 4)) {
    speedEgg += 1;
    lcd.Asc_String(20, 20 , Asc("Trung tang toc"), BLACK);
    lcd.display();
    delay(1000);
    lcd.fillrect(20, 20, 40, 20, DELETE);
  }

  if ((huong_cu == 4) && (ton_tai == 1)) {
    //trứng rơi xuống dưới;
    y_moi = y_cu + speedEgg;
    x_moi = x_cu;
    huong_moi = huong_cu;
    ve_trung(x_moi, y_moi);
  }

  //ghi tt
  obj_ga_trung[i][1] = x_moi;
  obj_ga_trung[i][2] = y_moi;
  obj_ga_trung[i][3] = huong_moi;
}

void ve_trung_roi() {
  for ( byte i = 1; i <= egg_max; i++) { // xét lần lượt từng quả trứng
    //vẽ trứng rơi
    if (obj_ga_trung[i][0] == 0) {
      // nếu không tồn tại trứng thì phải tạo trứng
      //b1: chuẩn bị sẵn tọa độ  bằng random
      randomSeed( millis() % 5000);
      byte X_random;
      X_random = random(Xmin, Xmax);
      // lưu thông tin
      obj_ga_trung[i][1] = X_random;
      //chương trình chỉ chạy 1 lần mà thôi
    }

    //khi gà đi đến vị trí chuẩn bị sẵn này, trứng bắt đầu được tạo và rơi
    if (obj_ga_trung[i][1] == obj_ga_trung[0][1]) { // thỏa mãn trùng tọa độ
      //b2: khởi tạo đối tượng
      // cấp thuộc tính cho đối tượng trứng
      //(nên nhớ đã có hoành độ Random)
      obj_ga_trung[i][2] = Ymin + 10; // tung độ ban đầu
      obj_ga_trung[i][3] = 4 ; //hướng xuống dưới
      obj_ga_trung[i][0] = 1; //cấp quyền tồn tại

    }

    //b2 : trứng roi
    //lấy thông tin

    xoa_trung(obj_ga_trung[i][1], obj_ga_trung[i][2]); //xóa ảnh cũ
    di_chuyen_trung(obj_ga_trung[i][3], i); // vẽ ảnh mới
    //b3: Trứng chạm đất và vỡ
    // hàm test nếu tung độ quá biên thì mất quả trứng đó
    if ( obj_ga_trung[i][2] >= Ymax) {
      obj_ga_trung[i][0] = 0; // xóa quyền tồn tại
      mang--;//trừ điểm
    }
  }//for
}

////==============================================================

////////ĐIỀU KHIỂN 2 NHÂN VẬT//////////////////////
void ve_nhan_vat(byte x, byte y, byte huong) {

  // ve nhan vat
  if (huong == 1) {
    //quay về bên phải
    lcd.Plus_Bitmap(x, y, 22, 18, nhan_vat,  0, NO_MIRROR, BLACK);
  }
  if (huong == 3) {
    //quay về bên trái
    lcd.Plus_Bitmap(x, y, 22, 18, nhan_vat, 0, MIRROR, BLACK);
  }
  lcd.display();
}

// xóa nhân vật
void xoa_nhan_vat(byte x, byte y) {
  lcd.fillrect(x, y - 7, 24, 19 + 7, DELETE);
  lcd.display();

}

void di_chuyen_nhan_vat() {
  byte x_cu, y_cu, huong_cu;
  byte x_moi;
  //lấy thông tin
  x_cu = obj_nhan_vat[1];
  y_cu = obj_nhan_vat[2];
  huong_cu = obj_nhan_vat[3];
  if (huong_cu == 1) {
    //đi về bên phải
    if (x_cu > Xmax - 10) {
      x_moi = x_cu - 3;
    } else
      x_moi = x_cu + 3;
    ve_nhan_vat(x_moi, y_cu, huong_cu);
  }

  if (huong_cu == 3) {
    //đi về bên trái

    if (x_cu < 3) {
      //chống quá biên
      x_moi = x_cu + 2;
    } else {
      x_moi = x_cu - 3;
    }
    ve_nhan_vat(x_moi, y_cu, huong_cu);
  }
  //lưu thông tin
  obj_nhan_vat[1] = x_moi;

}//di chuyển nhân vật
void xu_li_nhan_vat() {

  if (obj_nhan_vat[0] == 0) {
    // nếu không tồn tại gà thì phải tạo gà
    // cấp thuộc tính cho đối tượng gà
    obj_nhan_vat[1] = Xmin + 50; //hoành độ ban đầu
    obj_nhan_vat[2] = Ymax - 19; // tung độ ban đầu
    obj_nhan_vat[3] = 1 ; //hướng về bên phải
    obj_nhan_vat[0] = 1; //cấp quyền tồn tại
  }
  //b4: vẽ  nhân vật
  ve_nhan_vat(obj_nhan_vat[1], obj_nhan_vat[2], obj_nhan_vat[3]);

  //b5: điều khiển hướng nhân vật i

  if ( analogRead(dieu_khien) > 650) { // nút right
    //ghi lại tt hướng
    obj_nhan_vat[3] = 1;
    xoa_nhan_vat(obj_nhan_vat[1], obj_nhan_vat[2]); //xóa ảnh cũ
    //lấy thông tin tọa độ để xóa
    di_chuyen_nhan_vat();// vẽ ảnh mới
  }

  if ( analogRead(dieu_khien) < 400) { // nút left
    obj_nhan_vat[3] = 3;
    xoa_nhan_vat(obj_nhan_vat[1], obj_nhan_vat[2]); //xóa ảnh cũ
    //lấy thông tin tọa độ để xóa
    di_chuyen_nhan_vat();// vẽ ảnh mới
  }

}//xử lí nhân vật
//==================================================
////////////XỦ LÝ TRỨNG VÀ ĐIỂM//////////////////////

boolean va_cham(byte i) {
  // kiểm tra va trạm của quả trứng i với rổ
  byte x_vot, y_vot;
  if (obj_nhan_vat[3] == 3) {
    x_vot = obj_nhan_vat[1] + 7;
  }
  // nếu  sang phải thì hoành độ thêm 1 đoạn
  if (obj_nhan_vat[3] == 1) {
    x_vot = obj_nhan_vat[1] + 18;
  }
  y_vot = obj_nhan_vat[2] + 12;
  // nếu tọa độ vợt trù tọa độ trứng mà dương và trong 1 khoảng cố định
  int hieu_x, hieu_y;
  hieu_x = x_vot - obj_ga_trung[i][1];
  hieu_y = y_vot - obj_ga_trung[i][2];
  if ((hieu_x >= 0) && (hieu_y >= 0) && (hieu_x <= 8) && (hieu_y <= 8)) {
    return 1;// trả về true

  } else {
    return 0;// false
  }
}
void xu_ly_trung() {
  for (byte i = 1; i <= egg_max; i++) { // xét lần lượt từng quả trứng
    if (obj_ga_trung[i][0] == 0) {
      continue;
    } else {
      if (va_cham(i) == 1) { 
        obj_ga_trung[i][0] = 0; // xóa quyền tồn tại quả i
        diem++;//cộng 1 điểm
      }
      
      //đỡ hụt
      if ( obj_ga_trung[i][2] >= Ymax - 5) {
        obj_ga_trung[i][0] = 0; // xóa quyền tồn tại quả i
        xoa_trung(obj_ga_trung[i][1], obj_ga_trung[i][2]);
        if (obj_ga_trung[i][3] == 4) {
          mang -= 1; //trừ 1 mang
        }
      }
    }//if
  }//for
}
//=============================================

void in_diem() {
  lcd.fillrect(Xmin + 30, Ymin + 10 , 100, 64, DELETE);
  lcd.Asc_String(Xmin + 2, Ymin , Asc("Diem"), BLACK);
  lcd.Number_Long(Xmin + 30, Ymin, diem, ASCII_NUMBER, BLACK);
  lcd.drawline(Xmin + 2, Ymin + 8, Xmin + 40, Ymin + 8, BLACK);
  lcd.Asc_String(Xmin + 2, Ymin +  10 , Asc("Mang"), BLACK);
  lcd.Number_Long(Xmin + 30, Ymin + 10, mang, ASCII_NUMBER, BLACK);

  lcd.Asc_String(Xmax - 30, Ymin + 2, Asc("Cap:"), BLACK);
  lcd.Number_Long(Xmax - 8, Ymin + 2, cap, ASCII_NUMBER, BLACK);

  lcd.display();
}

//===========================================================


void lap_vo_han() {
  //thoát lặp khi nút Start! được nhấn
  song = 0;
  if (checkStart == 1) {
    lcd.Asc_String(40, 20 , Asc("Gameover!!!"), BLACK);
    lcd.Asc_String(45, 30 , Asc("Diem: "), BLACK);
    lcd.Number_Long(75, 30, diem, ASCII_NUMBER, BLACK);
    song = 2;
  }
  lcd.Asc_String(10, 57 , Asc("Start!"), BLACK);
  lcd.display();
  sing(song);
  while (digitalRead(start) != 0) {
    lcd.rect(8, 55, 40, 10, BLACK);
    lcd.display();
    if (digitalRead(start) == 0) {
      break;// thoát ngay
    }
    delay(250);
    lcd.rect(8, 52, 40, 10, DELETE);
    lcd.display();

    if (digitalRead(start) == 0) {
      break;// thoát ngay;
    }
    delay(250);
  }
}//đóng lặp vô hạn
//=========================
void reset() {
  mang = 3;
  diem = 0;
  speedChicken = 1;
  speedEgg = 1;
  cap = 1;
  for (byte i = 0; i <= egg_max; i++) {
    for (byte j = 0; j <= 4; j++) {
      obj_ga_trung[i][j] = 0;
    }
  }
  for (byte i = 0; i <= 1; i++) {
    for (byte j = 0; j <= 3; j++) {
      obj_nhan_vat[j] = 0;
    }
  }

}
//======================
void game_over( int mangchoi) {
  if (mangchoi <= 0) {
    lcd.clear();
    lap_vo_han();
    reset();
    lcd.clear();
  }
}
//=======================
void loop() {
  checkStart = 1;
  ga_di_va_de_trung();
  ve_trung_roi();
  xu_li_nhan_vat();
  xu_ly_trung();
  in_diem();
  game_over(mang);

}

void sing(int s) {
  if (s == 2) {
    int size = sizeof(melody_gameover) / sizeof(int);
    for (int thisNote = 0; thisNote < size; thisNote++) {
      int noteDuration = 1000 / tempo_gameover[thisNote];
      buzz(melodyPin, melody_gameover[thisNote], noteDuration);
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      buzz(melodyPin, 0, noteDuration);
    }
  }
  if (s == 1) {
    int size = sizeof(melody_start) / sizeof(int);
    for (int thisNote = 0; thisNote < size; thisNote++) {
      int noteDuration = 1000 / tempo_start[thisNote];
      buzz(melodyPin, melody_start[thisNote], noteDuration);
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      buzz(melodyPin, 0, noteDuration);
    }
  }
}

void buzz(int targetPin, long frequency, long length) {
  digitalWrite(13, HIGH);
  long delayValue = 1000000 / frequency / 2;
  long numCycles = frequency * length / 1000;
  for (long i = 0; i < numCycles; i++) { // for the calculated length of time...
    digitalWrite(targetPin, HIGH); // write the buzzer pin high to push out the diaphram
    delayMicroseconds(delayValue); // wait for the calculated delay value
    digitalWrite(targetPin, LOW); // write the buzzer pin low to pull back the diaphram
    delayMicroseconds(delayValue); // wait again or the calculated delay value
  }
  digitalWrite(13, LOW);

}
