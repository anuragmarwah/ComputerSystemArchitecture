#include<iostream>
//#include <stdio.h>
#include <string>
#include<vector>
#include<bitset>
#include<fstream>
#include <math.h>
using namespace std;
#define MAXINSTRUCTIONS 10000 // Currently supporting 10,000 instructions in the trace file
#define nullptr NULL

class Prediction
{
    public:
        Prediction(char *s1, char *s2)
        {
            m = 0;
            k = 0;
            numberofinstructions=0;
            mispredictions=0;
            //long int p;
            ifstream config;
            ifstream tracein;
            string line,line1,line2;
            config.open(s1); // "config.txt"
            if(config.is_open())
            {
                if(getline(config,line1))
                {
                    m=stoi(line1,nullptr,10);
                    if(getline(config,line2))
                        k=stoi(line2,nullptr,10);
                    else
                        cout << "Incomplete Configuration!" << endl;
                }
                else cout << "No Configuration found!" << endl;
            }
            else cout << "Unable to open Configuration File";
            config.close();
            
            BHR.resize(k);
            p = (long int)(pow(2,(m+k)));
            PHT.resize(p);
            cout << "m = " << m << " k = " << k << endl << "p = 2 ^ (m+k) = " << p << endl;
            
            trace.resize(MAXINSTRUCTIONS);
            tracein.open(s2); // "trace.txt"
            if(tracein.is_open())
            {
                while(getline(tracein,line))
                {
                    trace[numberofinstructions] = line;
                    numberofinstructions++;
                }
            }
            else cout << "Unable to open Trace File";
            tracein.close();
            
            cout << "Number of Instructions = " << numberofinstructions << endl;
            Predictor.resize(numberofinstructions);
            
            init();
        }
        void init()
        {
            int i,j;
            // Initialize BHR
            for (i=0;i<k;i++)
                BHR[i] = bitset<1> (string("1"));
            
            // Initialize PHT
            for (j=0;j<p;j++)
                PHT[j] = bitset<2> (string("11"));
            
            //for (j=0;j<p;j++)
            //    cout << PHT[j] << endl;
            
            for (i=0;i<numberofinstructions;i++)
                Predictor[i]=bitset<1> (string("1"));
            
            return;
        }
        float predict()
        {
            int i;
            
            // sample trace out
            // for (i=0;i<numberofinstructions;i++)
            //    Predictor[i] = bitset<1> (stoi(trace[i].substr(9,1),nullptr,2));
            bitset<32> PC;
            bitset<32> currentBHR;
            bitset<32> predictionaddress;
            bitset<2> predictedvalue;
            bitset<1> actualvalue;
            long predictaddr;
            float mispredictionrate=0;
            for (i=0;i<numberofinstructions;i++)
            {
                PC = getPC(i);
                currentBHR = getBHR();
                predictionaddress = bitset<32> ((currentBHR.to_string()).substr((32-k),k)+(PC.to_string()).substr((32-m),m));
                //cout << "Prediction Address: " << predictionaddress << endl;
                
                predictaddr = predictionaddress.to_ulong();
                predictedvalue = PHT[predictaddr];
                Predictor[i][0] = predictedvalue[1];
                
                actualvalue = bitset<1> (trace[i].substr(9,1));
                updateBHR(actualvalue);
                if(Predictor[i][0] != actualvalue[0])
                    mispredictions++;
                
                updatePHT(predictaddr,actualvalue);
                
                //cout << "Current Prediction: " << Predictor[i][0] << endl;
                //cout << "Actual Value: " << actualvalue << endl;
                //cout << "Current Misprediction Rate = " << (mispredictions * 100.0)/ (i+1) << endl;
            }
            mispredictionrate = (mispredictions * 100.0) / numberofinstructions;
            return mispredictionrate;
        }
        bitset<32> getPC(int i)
        {
            bitset<32> PC;
            PC = bitset<32> (((bitset<16> (stoi((trace[i]).substr(0,4),nullptr,16))).to_string())+((bitset<16> (stoi((trace[i]).substr(4,4),nullptr,16))).to_string()));
            return PC;
        }
        bitset<32> getBHR()
        {
            int i;
            bitset<32> currentBHR (0);
            for (i=k-1;i>=0;i--)
                currentBHR[i] = BHR[i][0];
            return currentBHR;
        }
        void updateBHR(bitset<1> currentvalue) // BHR [0] will be dropped away, BHR [k-1] will get the latest value of Branch Status
        {
            int i;
            for (i=0;i<=k-2;i++)
                BHR[i][0] = BHR[i+1][0];
            if(k!=0)
                BHR[k-1][0] = currentvalue[0];
            return;
        }
        void updatePHT(int index,bitset<1> actualvalue)
        {
            string newvalue;
            int currentvalue = stoi(PHT[index].to_string(),nullptr,2);
            //cout << "Current PHT value = " << currentvalue << endl;
            switch (currentvalue)
            {
                case 0: // Predict Not Taken with Strong Confidence
                    if (actualvalue.test(0)) // 1 = true
                        newvalue = "01";
                    else
                        newvalue = "00";
                    break;
                case 1: // Predict Not Taken with Weak Confidence
                    if (actualvalue.test(0)) // 1 = true
                        newvalue = "11";
                    else
                        newvalue = "00";
                    break;
                case 2: // Predict Taken with Weak Confidence
                    if (actualvalue.test(0)) // 1 = true
                        newvalue = "11";
                    else
                        newvalue = "00";
                    break;
                case 3: // Predict Taken with Strong Confidence
                    if (actualvalue.test(0)) // 1 = true
                        newvalue = "11";
                    else
                        newvalue = "10";
                    break;
            }
            PHT[index] = bitset<2> (newvalue);
            //cout << "Updated PHT value = " << PHT[index] << endl;
        }
        void outputtrace()
        {
            int i;
            ofstream traceout;
            
            traceout.open("trace.txt.out");
            if(traceout.is_open())
            {
                for (i=0;i<numberofinstructions;i++)
                    traceout << Predictor[i] << endl;
            }
            else cout << "Unable to open Trace output file";
            traceout.close();
            return;
        }
        ~Prediction()
        {
            PHT.resize(0);
            BHR.resize(0);
            Predictor.resize(0);
            trace.resize(0);
        }
    private:
        vector<bitset<2> > PHT;
        vector<bitset<1> > BHR;
        vector<bitset<1> > Predictor;
        vector<string> trace;
        int m;
        int k;
        long int p;
        int numberofinstructions;
        int mispredictions;
};

int main(int argc, char **argv)
{
    float rate;
    //cout << argv[0] << endl << argv[1] << endl << argv[2] << endl;
    //cout << argc<< endl;
    if (argc < 3)
        cout << "Too few arguments. Terminating..." << endl;
    else
    {
        if (argc > 3)
            cout << "Too many arguments. Will consider only first two given file names." << endl;
        Prediction myPrediction(argv[1],argv[2]);
        rate = myPrediction.predict();
        myPrediction.outputtrace();
        cout << "Mis-Prediction Rate = " << rate << endl;
    }
    return 0;
}
