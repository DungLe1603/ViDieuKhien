// toneeS  ==========================================
// Start by defining the relationship between 
//       note, period, &  frequency. 
#define  c     3830    // 261 Hz 
#define  d     3400    // 294 Hz 
#define  e     3038    // 329 Hz 
#define  f     2864    // 349 Hz 
#define  g     2550    // 392 Hz 
#define  a     2272    // 440 Hz 
#define  b     2028    // 493 Hz 
#define  C     1912    // 523 Hz 


// SETUP ============================================
// Set up speaker on a PWM pin (digital 9, 10 or 11)
int speakerOut = 11;
// Do we want debugging on serial out? 1 for yes, 0 for no
int DEBUG = 1;



// MELODY and TIMING  =======================================
//  melody[] is an array of notes, accompanied by beats[], 
//  which sets each note's relative length (higher #, longer note)
//trung vo 
int melody[] = {   c };
int beats[]  = {  40 }; 

//trung vao ro
//int melody2[] = {  b, C };
//int beats2[]  = {  8,24 }; 
//////////////////////////////////////
//int melody3[] = {  C,  b,  g,  C,  b,   e,  R,  C,  c,  g, a, C };
//int beats3[]  = { 16, 16, 16,  8,  8,  16, 32, 16, 16, 16, 8, 8 }; 
int  MAX_COUNT = sizeof(melody) / 2; // Melody length, for looping.

// Set overall tempo
long tempo = 10000;
// Set length of pause between notes
int  pause = 1000;
// Loop variable to increase Rest length
int  rest_count = 100; //<-BLETCHEROUS HACK; See NOTES

// Initialize core variables
int  tonee = 0;
int  beat = 0;
long duration  = 0;

// PLAY tonee  ==============================================
// Pulse the speaker to play a tonee for a particular duration
void playtonee() {
  long elapsed_time = 0;
  if (tonee > 0) { // if this isn't a Rest beat, while the tonee has 
    while (elapsed_time < duration) {
      digitalWrite(speakerOut,HIGH);
      delayMicroseconds(tonee / 2);
      // DOWN
      digitalWrite(speakerOut, LOW);
      delayMicroseconds(tonee / 2);
      elapsed_time += (tonee);
    } 
  }
  else { 
    for (int j = 0; j < rest_count; j++) { // See NOTE on rest_count
      delayMicroseconds(duration);  
    }                                
  }                                 
}
void setup() { 
  pinMode(speakerOut, OUTPUT);
  for (int i=0; i<MAX_COUNT; i++) {
    tonee = melody[i];
    beat = beats[i];

    duration = beat * tempo; // Set up timing

    playtonee(); 
    delayMicroseconds(pause);
  }
}
// LET THE WILD RUMPUS BEGIN =============================
void loop() {
  
}

