#include "Evaluation.hpp"
#include "Parameter.hpp"
#include <iostream>

using namespace std;

Evaluation::Evaluation(Predictor * pre, Model * mod)
{
	pPredictor = pre;
	pModel = mod;
}

Evaluation::~Evaluation()
{}

double Evaluation::calAccuracy(const vector<int> & newLabel,
		const vector<int> & orgLabel)
{
	double accuracy = 0;
	for(size_t i = 0; i < newLabel.size(); i++){
		accuracy += (newLabel[i] == orgLabel[i]);
	}
	return accuracy / newLabel.size();
}

pair<int, double>  Evaluation::calFeedback(const Sentence & sen, WordAgent * wa, vector<int> & standard)
{
        //cout<<"id is "<<wa->getID()<<", ";
        if(sen.size() == 0)
        {
                cout<<"sentence is null"<<endl;
        }
        int a;
	vector<double> tmp = pModel->getFeatureWeight();
	vector<int> father;
	double value = pPredictor->predict(sen,father);
	pair<int, double> p;

	/*for(size_t m = 0 ; m < father.size(); m++)
	{
	        cout<<father[m]<<" ";
        }
        */

	//cout<<standard.size()<<" "<<father.size()<<endl;
	double accuracy = calAccuracy(father, standard);
	if(accuracy > 0.8)
	{
	        p.first = 1;
                p.second = accuracy;

                return p;

        }
	map<int, double> tmpDomFeature = wa->getTmpReceptor();
	pModel->updateFeatureWeight(tmpDomFeature);
	vector<double> tmp2 = pModel->getFeatureWeight();
	/*for(size_t i = 0; i < tmp.size(); i++)
	{
	        if(tmp[i] != tmp2[i] )
	        {
	                cout<<tmp[i] <<" "<<tmp2[i]<<endl;
                }
        }
        */
        //cin>>a;
	vector<int> mutatefather;
	double mutatevalue = pPredictor->predict(sen,mutatefather);
	/*cout<<"mutate father is "<<endl;
	for(size_t n = 0 ; n < mutatefather.size(); n++)
	{
	        cout<<mutatefather[n]<<" ";
        }
        */

	double mutateaccuracy = calAccuracy(mutatefather, standard);

	//cin>>a;
	int differ = int((mutateaccuracy - accuracy) * PRECISION);
	pModel->setFeatureWeight(tmp);

	if((differ == 0) && (value == 1.0))
	{
		int d = int((mutateaccuracy - accuracy) * PRECISION);
		if(d > 0)
		{
			p.first = 2;
			p.second = mutatevalue;
			cout<<endl<<"id is "<<wa->getID()<<"	, ";
			cout<<"mst vs mmst : ("<<value<<"	: "<<mutatevalue<<"	) ; ";
                        cout<<"accuracy vs maccuracy : ("<<accuracy<<"	: "<<mutateaccuracy<<"	);"<<endl;
			return p;
		}
	}
	else if(differ > 0)
	{
	        int d = int((mutateaccuracy - accuracy) * PRECISION);
		if(d > 0)
		{
                        p.first = 1;
                        p.second = mutateaccuracy;
                        cout<<endl<<"id is "<<wa->getID()<<", ";
                        cout<<"mst vs mmst : ("<<value<<" : "<<mutatevalue<<") ; ";
                        cout<<"accuracy vs maccuracy : ("<<accuracy<<" : "<<mutateaccuracy<<") ;"<<endl;
                        return p;
		}
	}

	/*if(differ > 0)
	{
	        p.first = 1;
                p.second = mutateaccuracy;
                cout<<endl<<"id is "<<wa->getID()<<", "<<"Ag id is "<<wa->getAGID()<<",";
                cout<<"mst vs mmst : ("<<value<<" : "<<mutatevalue<<") ; ";
                cout<<"accuracy vs maccuracy : ("<<accuracy<<" : "<<mutateaccuracy<<") ;"<<endl;
                return p;
        }
        */

	p.first = -1;
	p.second =  0.0;


	return p;

}
