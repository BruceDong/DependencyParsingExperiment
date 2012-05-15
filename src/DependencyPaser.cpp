#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <cassert>
#include <cstring>
#include <cstdlib>

#include "DependencyPaser.hpp"
#include "Parameter.hpp"

using namespace std;

DependencyPaser::DependencyPaser()
{
	pModel = new Model();
	pPredictor = new Predictor((Model *)pModel);
	pEvaluation = new Evaluation((Predictor *) pPredictor,(Model *) pModel);
	pTrainer = new Trainer((Model *) pModel, (Evaluation *) pEvaluation);
}

DependencyPaser::~DependencyPaser()
{
	delete pModel;
	delete pTrainer;
	delete pPredictor;
}

bool DependencyPaser::loadModel(const char * file)
{
	return true;
}

bool DependencyPaser::saveModel(const char * file)
{
	return true;
}

bool DependencyPaser::_readFileAddBCell(const char * file)
{
	ifstream fin(file);
	string line;
	vector<vector<string> > senes;
	while(getline(fin, line)){
		if(line == ""){
			vector<int> father;
			Sentence sen;
			sen.push_back(make_pair("ROOT", "ORG"));
			father.push_back(-1);
			for(size_t i = 0; i < senes.size(); i++){
				sen.push_back(make_pair(senes[i][1], senes[i][3]));
				father.push_back(atoi(senes[i][6].c_str()));
			}
			//cout<<"word pairs in a sentence is "<<sen.size()<<endl;
			pModel->getFeatures(sen,senes,father);
			pTrainer->addBCells(sen, father);
			cout<<".";
			senes.clear();
		}
		else{
			vector<string> item;
			string tmp;
			istringstream sin(line);
			while(sin >> tmp){
				item.push_back(tmp);
				//cout<<tmp<<" ";
			}
			//cout<<endl;
			senes.push_back(item);
		}
	}
	//cout<<"Relicating B cells according to word frequences...";
        pTrainer->cloneBCells();
        //cout<<"Replicate finished!"<<endl;

	return true;
}

bool DependencyPaser::_readFileTrain(const char * file)
{
	pTrainer->constructBcellNet();

	string line;
	vector<vector<string> > senes;
	pModel->initFeatureWeight();
	for(size_t i = 0; i < LEARNTIMES; i++)
	{
	        ifstream fin(file);
                while(getline(fin, line)){
                        if(line == ""){
                                vector<int> father;
                                Sentence sen;
                                sen.push_back(make_pair("ROOT", "ORG"));
                                father.push_back(-1);
                                for(size_t i = 0; i < senes.size(); i++){
                                        sen.push_back(make_pair(senes[i][1], senes[i][3]));
                                        father.push_back(atoi(senes[i][6].c_str()));
                                }

                                pTrainer->rfTrain(sen, father);
                                senes.clear();
                                //break;
                        }
                        else{
                                vector<string> item;
                                string tmp;
                                istringstream sin(line);
                                while(sin >> tmp){
                                        item.push_back(tmp);
                                }
                                senes.push_back(item);
                        }
                }
                fin.close();
	}


	return true;
}
bool DependencyPaser::trainFile(const char * file)
{
        cout<<"Initilizing B cell Network...";
	_readFileAddBCell(file);
	cout<<"Initilizing finished!"<<endl;
	cout<<"Online learning...";
	_readFileTrain(file);
	cout<<"Online learning finished!"<<endl;

	return true;
}

bool DependencyPaser::predictFile(const char * testFile, const char * outFile)
{
        cout<<"Predicting...";
        vector<double> fw =  pModel->getFeatureWeight();
        /*cout<<"fw size "<<fw.size()<<endl;
        for(size_t i = 0; i < fw.size();i++)
        {
                if(fw[i] != 0.0)
                {
                        cout<<"id "<<i<<" "<<fw[i]<<" ";
                }
        }
        int a;
        cin>>a;
        */
	ifstream fin(testFile);
	ofstream fout(outFile);
	string line;
	vector<vector<string> > senes;
	while(getline(fin, line)){
		if(line == ""){
		        cout<<".";
			vector<int> father;
			Sentence sen;
			sen.push_back(make_pair("ROOT", "ORG"));
			for(size_t i = 0; i < senes.size(); i++){
				sen.push_back(make_pair(senes[i][1], senes[i][3]));
			}
			predict(sen, father);
			for(size_t i = 0; i < senes.size(); i++){
				senes[i][7] = "-";
				fout << senes[i][0];
				for(size_t j = 1; j < senes[i].size(); j++){
					if(j == 6) fout << "\t" << father[i+1];
					else fout << "\t" << senes[i][j];
				}
				if(father[i+1] == atoi(senes[i][6].c_str()))
				{
				        fout<< "\t1";
                                }
                                else
                                {
                                        fout<< "\t0";
                                }
				fout << endl;
			}
			fout << endl;
			senes.clear();
		}
		else{
			vector<string> item;
			string tmp;
			istringstream sin(line);
			while(sin >> tmp){
				item.push_back(tmp);
			}
			senes.push_back(item);
		}
	}
	cout<<endl<<"Predicting finished!"<<endl;
	return true;
}

double DependencyPaser::predict(const Sentence & sen, vector<int> & fa)
{
	return pPredictor->predict(sen, fa);
}

double DependencyPaser::evaluate(const char * outFile, const char * evaluateFile)
{
        cout<<"Evaluating..."<<endl;
        double accuracy = 0.0;
        int all = 0;
        int correct = 0;
        ifstream fin(outFile);
        ofstream fout(evaluateFile);
        string line;
        while(getline(fin,line))
        {
                if(line != "")
                {
                        vector<string> item;
			string tmp;
			istringstream sin(line);
			while(sin >> tmp){
				item.push_back(tmp);
			}

			if(atoi(item[(int)item.size() - 1].c_str()) == 1)
			{
			        correct++;
                        }
                        all++;
                }
        }

        accuracy = (double)correct/(double)all;
        cout<<"Evaluating finished!"<<endl;
        cout<<"Accuracy is "<<accuracy<<endl;
        fout<<"Accuracy is "<<accuracy<<endl;
        return accuracy;
}


