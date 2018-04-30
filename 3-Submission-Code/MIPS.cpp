#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>
using namespace std;
#define ADDU 1
#define SUBU 3
#define AND 4
#define OR  5
#define NOR 7
#define MemSize 65536 // memory size, in reality, the memory size should be 2^32, but for this lab, for the space resaon, we keep it as this large number, but the memory is still 32-bit addressable.
#define nullptr NULL

class RF
{
    public:
        bitset<32> ReadData1, ReadData2; 
     	RF()
    	{ 
          Registers.resize(32);  
          Registers[0] = bitset<32> (0);  
        }
	
        void ReadWrite(bitset<5> RdReg1, bitset<5> RdReg2, bitset<5> WrtReg, bitset<32> WrtData, bitset<1> WrtEnable)
        {
            if(WrtEnable==1){ // write before read
                Registers[WrtReg.to_ulong()]=WrtData;
                cout << "Writing data to RF..." << WrtData << endl;
            }
            ReadData1=Registers[RdReg1.to_ulong()];
            ReadData2=Registers[RdReg2.to_ulong()];
         }
		 
	void OutputRF()
             {
               ofstream rfout;
                  rfout.open("RFresult.txt",std::ios_base::app);
                  if (rfout.is_open())
                  {
                    rfout<<"A state of RF:"<<endl;
                  for (int j = 0; j<32; j++)
                      {        
                        rfout << Registers[j]<<endl;
                      }
                     
                  }
                  else cout<<"Unable to open file";
                  rfout.close();
               
               }     
	private:
            vector<bitset<32> >Registers;
	
};

class ALU
{
      public:
             bitset<32> ALUresult;
             bitset<32> ALUOperation (bitset<3> ALUOP, bitset<32> oprand1, bitset<32> oprand2)
             {
                 switch(ALUOP.to_ulong())
                 {
                     case ADDU: ALUresult = bitset<32> (oprand1.to_ulong() + oprand2.to_ulong());
                         cout << "Add Operation" << endl;
                         break;
                     case SUBU: ALUresult = bitset<32> (oprand1.to_ulong() - oprand2.to_ulong());
                         cout << "Sub Operation" << endl;
                         break;
                     case AND: ALUresult = bitset<32> (oprand1 & oprand2);
                         cout << "AND Operation" << endl;
                         break;
                     case OR: ALUresult = bitset<32> (oprand1 | oprand2);
                         cout << "OR Operation" << endl;
                         break;
                     case NOR: ALUresult = bitset<32> (~(oprand1 | oprand2));
                         cout << "NOR Operation" << endl;
                         break;
                     
                 }
                 
                 // implement the ALU operations by you. 
                 return ALUresult;
               }            
};

class INSMem
{
      public:
          bitset<32> Instruction;
          INSMem()
          {       IMem.resize(MemSize); 
                  ifstream imem;
                  string line;
                  int i=0;
                  imem.open("imem.txt");
                  if (imem.is_open())
                  {
                  while (getline(imem,line))
                     {      
                         IMem[i] = bitset<8>(std::stoi(line,nullptr,2));
                        i++;
                     }
                     
                  }
                  else cout<<"Unable to open file";
                  imem.close();
                     
                  }
                  
          bitset<32> ReadMemory (bitset<32> ReadAddress) 
              {    
               // implement by you. (Read the byte at the ReadAddress and the following three byte).
               //   long int addr = ReadAddress.to_ulong();
               //   std::string instr = IMem[addr].to_string() + IMem[addr+1].to_string() + IMem[addr+2].to_string() + IMem[addr+3].to_string();
               //   Instruction = bitset<32> (instr);
                  Instruction = bitset<32> (IMem[ReadAddress.to_ulong()].to_string() + IMem[ReadAddress.to_ulong()+1].to_string() + IMem[ReadAddress.to_ulong()+2].to_string() + IMem[ReadAddress.to_ulong()+3].to_string());
                  return Instruction;
              }     
      
      private:
           vector<bitset<8> > IMem;
      
};
      
class DataMem    
{
      public:
          bitset<32> readdata;  
          DataMem()
          {
             DMem.resize(MemSize); 
             ifstream dmem;
                  string line;
                  int i=0;
                  dmem.open("dmem.txt");
                  if (dmem.is_open())
                  {
                  while (getline(dmem,line))
                       {      
                        DMem[i] = bitset<8>(stoi(line,nullptr,2));
                        i++;
                       }
                  }
                  else cout<<"Unable to open file";
                  dmem.close();
          
          }  
          bitset<32> MemoryAccess (bitset<32> Address, bitset<32> WriteData, bitset<1> readmem, bitset<1> writemem) 
          {    
               
               // implement by you.
              unsigned long addr = stoi(Address.to_string(),nullptr,2);
              readdata=bitset<32> (0);
              if(writemem.test(0) && !(readmem.test(0))){
                  DMem[addr] = bitset<8> ((WriteData.to_string()).substr(0,8));
                  DMem[addr+1] = bitset<8> ((WriteData.to_string()).substr(8,8));
                  DMem[addr+2] = bitset<8> ((WriteData.to_string()).substr(16,8));
                  DMem[addr+3] = bitset<8> ((WriteData.to_string()).substr(24,8));
              }
              else if(readmem.test(0) && !(writemem.test(0))){
                  readdata = bitset<32> (DMem[(Address.to_ulong())].to_string() + DMem[(Address.to_ulong())+1].to_string() + DMem[(Address.to_ulong())+2].to_string() + DMem[(Address.to_ulong())+3].to_string());
              }
              return readdata;
              // what to return in case of writemem ??
          }   
                     
          void OutputDataMem()
          {
               ofstream dmemout;
                  dmemout.open("dmemresult.txt");
                  if (dmemout.is_open())
                  {
                  for (int j = 0; j< 1000; j++)
                       {     
                        dmemout << DMem[j]<<endl;
                       }
                     
                  }
                  else cout<<"Unable to open file";
                  dmemout.close();
               
               }             
      
      private:
           vector<bitset<8> > DMem;
      
};  

void dumpResults(bitset<32> pc, bitset<5> WrRFAdd, bitset<32> WrRFData, bitset<1> RFWrEn, bitset<32> WrMemAdd, bitset<32> WrMemData, bitset<1> WrMemEn)
{
                  ofstream fileout;
                  
                  fileout.open("Results.txt",std::ios_base::app);
                  if (fileout.is_open())
                  {
                  
                  fileout <<pc<<' '<<WrRFAdd<<' '<<WrRFData<<' '<<RFWrEn<<' '<<WrMemAdd<<' '<<WrMemData<<' '<<WrMemEn << endl;
                    
                  }
                  else cout<<"Unable to open file";
                  fileout.close();

}

bitset<32> decode(bitset<32> instr)
{
    bitset<32> controlword (0);
/*    if (instr [31:26] == 100011 | instr [31:26] == 101011)
        controlword [2:0] = 001;
    else if (instr [31:26] == 000000)
        controlword [2:0] = instr [2:0];
    else
        controlword [2:0] = instr [28:26]; */

    if (instr == 0)
        return controlword;
    string opcode((instr.to_string()).substr(0,6)); // Note: starting position
    // ALU-OP
    if (opcode == "100011" || opcode == "101011") {
        cout << "Load/Store Instruction" << endl;
        controlword [2] = 0;
        controlword [1] = 0;
        controlword [0] = 1;
        controlword [4] = 1; // I-type Instruction
        controlword [7] = 0; // Not a J-type instruction
        // IsLoad, IsStore
        if (opcode == "100011") { // Load Instruction
            controlword [5] = 1;
            controlword [6] = 0;
        }
        else if (opcode == "101011") { // Store Instruction
            controlword [6] = 1;
            controlword [5] = 0;
        }
        else {
            controlword [5] = 0;
            controlword [6] = 0;
        }
    }
    else if (opcode == "000000") { // R-type Instruction
        cout << "R-type Instruction" << endl;
        controlword [2] = instr [2];
        controlword [1] = instr [1];
        controlword [0] = instr [0];
        controlword [4] = 0; // Not an I-type instruction
        controlword [7] = 0; // Not a J-type instruction
    }
    else {
        //cout << "I-type or J-type Instruction" << endl;
        controlword [2] = instr [28];
        controlword [1] = instr [27];
        controlword [0] = instr [26];
        if (opcode == "001001" || opcode == "000100") {
            controlword [4] = 1; // I-type Instruction
            controlword [7] = 0; // Not a J-type instruction
            cout << "I-type Instruction" << endl;
            // isBranch
            if (opcode == "000100")
                controlword [8] = 1;
            else
                controlword [8] = 0;
        }
        else {
            controlword [4] = 0; // Not an I-type instruction
            controlword [7] = 1; // J-type Instruction
            cout << "J-type Instruction" << endl;
        }
    }
    
    // Write Enable
    if (controlword [6] == 1 || controlword [7] == 1 || controlword [8] == 1) // isStore | J-Type | isBranch
        controlword [3] = 0;
    else
        controlword [3] = 1;
    
    cout << "Opcode = " << opcode << endl;
    cout << "Control Word = " << controlword << endl;
    
    return controlword;
}
bitset<32> signextimm (bitset<32> ins)
{
    bitset<16> imm;
    bitset<32> extimm;
    imm = bitset<16> ((ins.to_string()).substr(16,16));
    if(imm.test(15))
        extimm = bitset<32> ("11111111"+imm.to_string());
    else
        extimm = bitset<32> ("00000000"+imm.to_string());
    cout << "Sign-Extended Imm = " << extimm << endl;
    return extimm;
}
bitset<32> nextpc(bitset<32> currentpc,bitset<32> inst,bitset<32> cword)
{
    bitset<32> newpc;
    if ((cword.test(8)) && (cword.test(9))) { // BEQ
        newpc = bitset<32> (currentpc.to_ulong() + (stoi((signextimm(inst).to_string()).substr(2,30)+"00",nullptr,2)));
    }
    else if(cword.test(7)) // J-type
        newpc = bitset<32> ((currentpc.to_string()).substr(0,4)+(inst.to_string()).substr(6,26)+"00");
    else
        newpc = bitset<32> (currentpc.to_ulong() + 4);
    cout << "Next PC = " << newpc << endl << endl;
    return newpc;
}
bitset<65> memstage(bitset<32> resultmem,bitset<32> ctrlwordmem,DataMem &myDataMem1,RF &myRF1)
{
    bitset<32> DMReadDataMEM (0);
    bitset<32> DMWriteData (0);
    if (ctrlwordmem.test(5)){ // isLoad
        cout << "Load Instruction" << endl;
        DMWriteData = bitset<32> (0);
        DMReadDataMEM = myDataMem1.MemoryAccess(resultmem,DMWriteData,1,0);
        return (bitset<65> ((bitset<33> (0)).to_string()+DMReadDataMEM.to_string()));
    }
    
    // Write Data Memory
    if (ctrlwordmem.test(6)){ // isStore
        cout << "Store Instruction" << endl;
        DMWriteData = myRF1.ReadData2;
        myDataMem1.MemoryAccess(resultmem,DMWriteData,0,1);
        return (bitset<65> ("1"+resultmem.to_string()+DMWriteData.to_string()));
    }
    return (bitset<65> (0));
    
}
bitset<38> wbstage(bitset<32> instrwb,bitset<32> ctrlwordwb,bitset<32> resultwb,bitset<32> DMReadDataWB,RF &myRF2)
{
    //cout << " Ctrl Word (WB Stage) = " << ctrlwordwb << endl;
    bitset<5> WrtRFRegWB;
    bitset<32> WrtRFDataWB;
    bitset<1> WrtRFEnableWB (0);
    bitset<38> RFWriteWB;
    if(!(ctrlwordwb.test(7)) && !(ctrlwordwb.test(8))) { // Not J-type and Not Branch
        if (ctrlwordwb.test(4)) // I-type
            WrtRFRegWB=bitset<5> ((instrwb.to_string()).substr(11,5));
        else // R-type
            WrtRFRegWB=bitset<5> ((instrwb.to_string()).substr(16,5));
        if (ctrlwordwb.test(5)) // isLoad
            WrtRFDataWB = DMReadDataWB;
        else
            WrtRFDataWB = resultwb;
        if(ctrlwordwb.test(3))
            WrtRFEnableWB=1;
        else
            WrtRFEnableWB=0;
        myRF2.ReadWrite(0,0,WrtRFRegWB,WrtRFDataWB,WrtRFEnableWB);
        cout << "Write RF Addr = " << WrtRFRegWB << endl;
        cout << "Write RF Data = " << WrtRFDataWB << endl;
        cout << "Write Enable = " << WrtRFEnableWB << endl;
    }
    //myRF2.OutputRF();
    RFWriteWB = bitset<38> (WrtRFEnableWB.to_string()+WrtRFRegWB.to_string()+WrtRFDataWB.to_string());
    return RFWriteWB;// myRF2;
}
bitset<32> exstage(bitset<32> instrEX,bitset<32> ctrlwordEX,RF &myRF3,ALU &myALU1)
{
    bitset<32> ALUoperand1,ALUoperand2;
    bitset<32> resultEX (0);
    if(!(ctrlwordEX.test(7)) && !(ctrlwordEX.test(9))) // Not J-type and Not BEQ
    {
        // EX Stage
       /* if(ctrlwordEX.test(8)) { // isBranch
            if (myRF3.ReadData1 == myRF3.ReadData2) //isEqual
                ctrlwordEX[9] = 1;
            else
                ctrlwordEX[9] = 0;
        }
        else
        {*/
            ALUoperand1=myRF3.ReadData1;
            if (ctrlwordEX.test(4)) // I-type
                ALUoperand2=signextimm(instrEX);
            else
                ALUoperand2=myRF3.ReadData2;
            resultEX=myALU1.ALUOperation(bitset<3> ((ctrlwordEX.to_string()).substr(29,3)),ALUoperand1,ALUoperand2);
            cout << "Result = " << resultEX << endl;
      //  }
    }
    return resultEX;
}
bitset<32> rfstage(bitset<32> instrRF,bitset<32> ctrlwordRF,RF &myRF4)
{
    bitset<5> RdRFReg1,RdRFReg2;
    if(!(ctrlwordRF.test(7))) // Not J-type
    {
        // RF Read Stage
        RdRFReg1=bitset<5> ((instrRF.to_string()).substr(6,5));
        RdRFReg2=bitset<5> ((instrRF.to_string()).substr(11,5));
        myRF4.ReadWrite(RdRFReg1,RdRFReg2,0,0,0);
        cout << "Read Reg 1 Addr = " << RdRFReg1 << endl;
        cout << "Read Reg 1 Data = " << myRF4.ReadData1 << endl;
        cout << "Read Reg 2 Addr = " << RdRFReg2 << endl;
        cout << "Read Reg 2 Data = " << myRF4.ReadData2 << endl;
        if(ctrlwordRF.test(8)) { // isBranch
            if (myRF4.ReadData1 == myRF4.ReadData2) //isEqual
                ctrlwordRF[9] = 1;
            else
                ctrlwordRF[9] = 0;
        }
        
    }
    
    return ctrlwordRF;
}

int main()
{
    bitset<32> pc=0;
    //bitset<32> nextpc;
    RF myRF;
    ALU myALU;
    INSMem myInsMem;
    DataMem myDataMem;

    //bitset<5> RdRFReg1,RdRFReg2;
    
    bitset<1> WrtRFEnable (0);
    bitset<5> WrtRFReg;
    bitset<32> WrtRFData;
    bitset<38> RFWrite (0);
    
    bitset<65> memresult (0);
    //bitset<32> ALUoperand1;
    //bitset<32> ALUoperand2;
    
    //bitset<1> isLoad,isStore;
    //bitset<1> readDM (0);
   // bitset<1> writeDM (0);
  
 
    bitset<32> WrtMemAddr,WrtMemData;
    bitset<1> WrtMemEnable;
    

    bitset<32> DMReadData;
  //  bitset<32> DMWriteData (0);
    bitset<32> instr_1 (0);
    bitset<32> ctrlword_1 (0);
    bitset<32> result_1;
    
    bitset<32> instr_2 (0);
    bitset<32> ctrlword_2 (0);
    bitset<32> result_2;
    
    bitset<32> instr_3 (0);
    
    //for(int i=0;i<10;i++)
    while (1) //each loop body corresponds to one clock cycle.
	{
        cout << "Stage 1" << endl;
        // Stage 1 - MEM/WB
        if(ctrlword_1.any()) {
            // MEM
            memresult=memstage(result_1,ctrlword_1,myDataMem,myRF);
            if (ctrlword_1.test(5)) // isLoad
                DMReadData = bitset<32> ((memresult.to_string()).substr(32,32));
            else if (ctrlword_1.test(6)){ // isStore
                WrtMemEnable = bitset<1> ((memresult.to_string()).substr(0,1));
                WrtMemAddr = bitset<32> ((memresult.to_string()).substr(1,32));
                WrtMemData = bitset<32> ((memresult.to_string()).substr(32,32));
            }
            // WB
            RFWrite=wbstage(instr_1,ctrlword_1,result_1,DMReadData,myRF);
        }
        
        cout << "Stage 2" << endl;
        // Stage 2 - ID/RF/EX
        // Decode
        ctrlword_2 = decode(instr_2);
        if(ctrlword_2.any()) {
            // RF Read
            ctrlword_2 = rfstage(instr_2,ctrlword_2,myRF);
            // EX
            result_2 = exstage(instr_2,ctrlword_2,myRF,myALU);
        }
        instr_1 = instr_2;
        ctrlword_1 =  ctrlword_2;
        result_1 = result_2;
        
        cout << "Stage 3" << endl;
        // Stage 3 - IF
        instr_3 = myInsMem.ReadMemory(pc);
        cout << "PC = " << pc << endl;
        cout << "Instruction = " << instr_3 << endl;
        if(ctrlword_1.test(7)){ // J-type
            instr_2 = bitset<32> (0);
        }
        else
            instr_2 = instr_3;
        
        
        // At the end of each cycle, fill in the corresponding data into "dumpResults" function to output files.
        // The first dumped pc value should be 0.
        cout << "RF Write = " << RFWrite << endl;
        WrtRFEnable = bitset<1> ((RFWrite.to_string()).substr(0,1));
        WrtRFReg = bitset<5> ((RFWrite.to_string()).substr(1,5));
        WrtRFData = bitset<32> ((RFWrite.to_string()).substr(6,32));
        dumpResults(pc,WrtRFReg,WrtRFData,WrtRFEnable,WrtMemAddr,WrtMemData,WrtMemEnable);
        
        
        
        if (instr_3 == 0xFFFFFFFF) {
            cout << "Halt Received. Terminating..." << endl;
            break;
        }
        // Increment PC
        pc=nextpc(pc,instr_1,ctrlword_1);
        //myRF.OutputRF(); // dump RF;
    }
    myRF.OutputRF(); // dump RF;
    myDataMem.OutputDataMem(); // dump data mem
    return 0;
        
}
