const double CyclesPerMicroSek = F_CPU/1000000;

const int samples = 44;
const double resolution = 8.1818181818181818; //Deg (360/44)
const unsigned long freq = 40000;//Hz
const double Tsek = 0.0000249; //sek 1/freq
const double TuSek = Tsek*1000*1000;
const double TperSampleMicro = TuSek/samples; //us
const double cylesPerSample = TperSampleMicro*CyclesPerMicroSek;
const unsigned long cyclesPerPeriod = TuSek*CyclesPerMicroSek;

const bool baseSignal[samples] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

const int pins[4] = {0,2,4,5};

void setup() {
  Serial.begin(9600);
  Serial.print("CPU Frequenz: ");
  Serial.println(F_CPU);
  Serial.print("Cycles Per Micro Second: ");
  Serial.println(CyclesPerMicroSek);

  Serial.print("Samples: ");
  Serial.println(samples);
    Serial.print("Resolution in deg: ");
  Serial.println(resolution);
  Serial.print("Signal Frequenzy: ");
  Serial.println(freq);
  Serial.print("Period Time Signal MicroSek: ");
  Serial.println(TuSek);
  Serial.print("Time Per Sample MicroSek: ");
  Serial.println(TperSampleMicro);
  Serial.print("Cyles Per Sample: ");
  Serial.println(cylesPerSample);
  Serial.print("Cyles Per Period: ");
  Serial.println(cyclesPerPeriod);

  pinMode(0,OUTPUT);
  pinMode(2,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);
}
void loop() {
  for(float i = -180;i<180; i=i+resolution){
    genWave(180+i);//0 = 15grad 180 = 0grad
  }

}

void genWave(float phase){//Phase in deg
  unsigned long t0 = ESP.getCycleCount();
  while(true){
    unsigned long CyclesNorm = ESP.getCycleCount()%cyclesPerPeriod; //Norm = [0-cyclesPerPeriod]
    unsigned long index = CyclesNorm/cylesPerSample; //Without Offset
    unsigned int offset = phase/resolution;

		unsigned long parallel1 = 0;
		unsigned long parallel0 = 0;

    //calc BitPattern
    for(int i = 0;i<8; i++){
      unsigned long IndexI = index+(offset*i);
      
      IndexI = IndexI%(samples-1);

      if(baseSignal[IndexI] == 1){
        parallel1 |= 1 << pins[i];
      }else{
        parallel0 |= 1 << pins[i];
      }
    }
    GPIO.out_w1tc = parallel1;
    GPIO.out_w1ts = parallel0;

    if((ESP.getCycleCount()-t0)>500000*CyclesPerMicroSek)break;//Break after one sek
  }
}
