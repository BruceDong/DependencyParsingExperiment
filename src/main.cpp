#include <iostream>
#include <cassert>

#include "DependencyPaser.hpp"
#include "Test.hpp"
#include <fstream>
#include <sstream>

using namespace std;

int main(int argc, char * argv[])
{
/*	assert(argc == 4);
	DependencyPaser dp;
	cout<<"load model"<<endl;
	dp.loadModel(argv[1]);
	cout << "model loaded!" << endl;
	dp.predictFile(argv[2], argv[3]);
	cout << "predicted OK" << endl;
*/
	testTrain();
	//testPredict();
	testEvaluation();
	/*fstream in("./result/fweight");
	vector<double> fw;
	if(in)
	{
	        string line;
	        vector<vector<string> > v;
	        while(getline(in,line))
	        {
	                vector<string> item;
			string tmp;
			istringstream sin(line);
			while(sin >> tmp){
				item.push_back(tmp);
				//cout<<tmp<<" ";
			}
			v.push_back(item);

                }

                for(size_t i = 0; i < v.size() - 1;i++)
                {
                        for(size_t j = i + 1; j < v.size();j++)
                        {
                                for(size_t k = 0; k < v[i].size(); k++)
                                {
                                        if(atof(v[i][k].c_str()) != atof(v[j][k].c_str()))
                                        {
                                                cout<<"k is "<<k<<" "<<v[i][k]<<" "<<v[j][k]<<endl;
                                        }
                                        else if((atof(v[i][k].c_str()) == atof(v[j][k].c_str())) && (atof(v[i][k].c_str()) > 0.0))
                                                {
                                                    cout<<"k is "<<k<<" "<<v[i][k]<<endl;;
                                                }
                                }

                        }
                }


        }
        */


	return 0;
}
