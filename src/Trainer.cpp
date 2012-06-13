#include "Trainer.hpp"
#include "Parameter.hpp"

#include <iostream>
#include <vector>
#include <fstream>

using namespace std;

Trainer::Trainer(Model * pm, Evaluation * eva) : pModel(pm)
{
	pEnv = new Environment(ROWS, COLS,eva,pm);
	simu = new Simulator(pEnv);
	pEva = eva;
}

Trainer::~Trainer()
{
	delete pEnv;
	delete simu;
}

bool Trainer::rfTrain(const Sentence & sen, const vector<int> & fa)
{
        pEnv->setSentence(sen);
        std::pair<Sentence, vector<int> > p;
        p.first = sen;
        p.second = fa;
        vSen.push_back(p);

        pEnv->setFather(fa);
        pEnv->setFeedback(false);
        /*vector<double> fw = pModel->getFeatureWeight();
        cout<<"feature weight size is "<<fw.size()<<endl;
        for(size_t i = 0; i < fw.size();i++)
        {
                if(fw[i] != 0.0)
                {
                        cout<<"("<<i<<","<<fw[i]<<")"<<" ";
                }
        }
        cout<<endl;
        */



        int a;
        //cin>>a;
	/*update weights of receptor(features) by learning from a sample*/
	/*injecting antigens*/
	//cout<<"B cell size is "<<pEnv->getBcellNum()<<endl;
	//cin>>a;
	//pEnv->initAntigenNum();
        _injectAntigen(sen, fa);
        //cout<<"AG size is "<<pEnv->getAntigenNum()<<endl;
        //cin>>a;

        cout<<"(s, "<<vSen.size()<<")";
	if(simu->run(sen,fa))
        {
                //cout<<"killed !";
                //cin>>a;
                //cout<<"ag number is "<<pEnv->getAntigenNum()<<endl;
                /*vector<double> fw = pModel->getFeatureWeight();
                for(size_t i = 0; i < fw.size();i++)
                {
                        if(fw[i] != 0.0)
                        {
                                cout<<"("<<i<<","<<fw[i]<<")"<<" ";
                        }
                }
                cout<<endl;
                Sentence sens = sen;
                vector<int> f = fa;
                pEva->evalute(sens,f);
                */
                cout<<endl;
                double sum = 0.0;
                for(size_t i = 0; i < vSen.size(); i++)
                {
                        cout<<"sen id "<<i+1;
                        Sentence sen = vSen[i].first;
                        vector<int> s = vSen[i].second;
                        sum += pEva->evalute(sen,s);
                        cout<<endl;
                }

                double average = sum/(double)vSen.size();
                cout<<"Average accuracy is "<<average<<endl;

                return true;
        }





	return false;
}

bool Trainer::addBCells(const Sentence & sen, const vector<int> & fa)
{
	vector<int> features;
	for(size_t i = 1; i < sen.size(); i++){
		int j = fa[i];
		int bi = _buildBCell(sen[i].first);
		int bj = _buildBCell(sen[j].first);
		pModel->getFeatureIDVec(sen, j, i, features);
		BCells[bi].addRecFeature(features);
		BCells[bj].addDomFeature(features);
	}
	return true;
}

int Trainer::_buildBCell(const string & word)
{

        if(wordID.find(word) == wordID.end()){
		wordID[word] = BCells.size();
		bool flag = true;
		while(flag)
		{
                        pair<int,int> pos = pEnv->getRandomPosition();
                        //if(pEnv->getLocalAgentsNum(pos) < MAXNUMAGENT)/*local number of agents must be lower than threshold*/
                        {
                                pEnv->setLocalAgentsNum(pos);
                                BCells.push_back(WordAgent(wordID[word], pEnv,simu,pos, BCELL,1));
                                flag = false;
                        }
		}
	}
	int res = wordID[word];
	//cout<<"B word "<<word<<" id "<<res<<endl;
	if((int)wordFreq.size() <= res){
		wordFreq.push_back(0);
	}
	wordFreq[res] ++;
	return res;
}

bool Trainer::constructBcellNet()
{
        cout<<"Constructing B cell network..."<<endl;
        cout<<"Number of B cells is "<<BCells.size()<<endl;

	for(size_t i = 0; i < BCells.size(); i++)
	{
	        //cout<<"id is "<<BCells[i].getID()<< " ";
	        //BCells[i].setStatus(ACTIVE);
	        //BCells[i].setCategory(BCELL);
		simu->addWordAgent(BCells[i]);
		//pEnv->increaseBcellNum();
	}

	BCells.clear();
	cout<<"Construct finished!"<<endl;
	return true;
}

bool Trainer::_buildAntigen(const Sentence & sen,int child,const std::string & word, int parent,const std::string & fword)
{
        Antigens.push_back(WordAgent(wordID[word],pEnv,simu,pEnv->getRandomPosition(), ANTIGEN,1));
        Antigens.push_back(WordAgent(wordID[fword],pEnv,simu,pEnv->getRandomPosition(), ANTIGEN,1));
        vector<int> features;
        pModel->getFeatureIDVec(sen, parent, child, features);
        //cout<<"feature size "<<features.size()<<endl;
        vector<WordAgent>::iterator it = Antigens.end();
        (--(--it));
        (*it).addRecFeature(features);
        //cout<<"ag id "<<(*it).getID()<<endl;
        Antigens.erase(Antigens.end());

	return true;
}

bool Trainer::_addAntigenToSimulator(const Sentence & sen, const std::vector<int> & fa)
{

	return true;
}

bool Trainer::_addAntigen()
{
        vector<vector<int> > grid =  pEnv->getGrid();
        vector<pair<int,int> > positions;
        //cout<<"grid is ";
        /*for(size_t i = 0; i < grid.size(); i++)
        {
                for(size_t j = 0; j < grid[i].size(); j++)
                {
                        if(grid[i][j] > 0)
                        {
                                positions.push_back(make_pair(i,j));
                                //cout<<" 1 ";
                                //cout<<grid[i][j]<<" ";
                        }
                        else
                        {
                                cout<<" 0 ";
                        }

                }
        }*/

        for(size_t i = 0; i < ROWS; i++)
        {
                for(size_t j = 0; j < COLS; j++)
                {
                        positions.push_back(make_pair(i,j));
                }
        }

        //cout<<endl;
        /*cout<<"agents are ";
        vector<map<int, WordAgent> > wa = pEnv->getAgents();
        for(size_t j = 0; j < wa.size(); j++)
        {
                cout<<wa[j].size()<<" ";
        }
        int count = 0;
        for(size_t j = 0; j < wa.size(); j++)
        {
                map<int, WordAgent>::iterator it = wa[j].begin();
                while(it != wa[j].end())
                {
                        if((it->second.getCategory() == ANTIGEN) && (it->second.getStatus() == DIE))
                        {
                                count++;
                        }
                        it++;
                }
        }
        cout<<"antigen number is "<<count<<endl;
        cout<<endl;
        */
        //cout<<"Antigen is as follow"<<endl;
        pEnv->initAgID();
        if(Antigens.size() > 0)
        {
                pEnv->setAntigenQuantity((int)Antigens.size());
                int ID = -1;
                int pos = -1;
                cout<<"ag size is "<<Antigens.size()<<endl;
                for(size_t p = 0; p < Antigens.size(); p++)
                {
                        pos = p%positions.size();
                        Antigens[p].setPosition(positions[pos]);
                        simu->addWordAgent(Antigens[p]);
                }
        }
        //cout<<endl;

        Antigens.clear();

        return true;
}

bool Trainer::_injectAntigen(const Sentence & sen, const std::vector<int> & fa)
{
	for(size_t i = 1; i < sen.size(); i++)
	{
		//int i = 3;
		int j = fa[i];
		_buildAntigen(sen,i,sen[i].first,j,sen[j].first);
		/*clone antigens*/
		cloneAntigens();
	}

	_addAntigen();

	return true;
}

bool Trainer::cloneAntigens()
{
        //cout<<"clone antigens"<<endl;
        //cout<<"size is "<<Antigens.size()<<endl;
        int l = (int)Antigens.size() - 1;
        //cout<<"l is "<<l<<endl;
        //cout<<"id "<<Antigens[l].getID()<<endl;
        for(size_t p = 1; p < ROWS * COLS; p++)
        {
                //cout<<"id is "<<Antigens[l].getID()<<endl;
                WordAgent wa(Antigens[l].getID(), pEnv,simu,Antigens[l].getPosition(), ANTIGEN,1);
                vector<int> rec = Antigens[l].getRecReceptor();
                //cout<<"size of recetor is "<<rec.size()<<endl;
                wa.setRecReceptor(rec);
                Antigens.push_back(wa);
        }

        return true;
}

bool Trainer::cloneBCells()
{
	for(size_t i = 0; i < wordFreq.size(); i++){
		for(int j = 1; j < wordFreq[i]; j++){

                        if(pEnv->getLocalAgentsNum(BCells[i].getPosition()) < MAXNUMAGENT)
                        {
                                pEnv->setLocalAgentsNum(BCells[i].getPosition());
                                WordAgent wa(BCells[i].getID(), pEnv,simu,BCells[i].getPosition(), BCELL,1);
                                map<int,double> m = BCells[i].getDomReceptor();
                                wa.setDomReceptor(m);
                                vector<int> v = BCells[i].getRecReceptor();
                                wa.setRecReceptor(v);
                                BCells.push_back(wa);
                        }
                        else
                        {
                                bool flag = true;
                                while(flag)
                                {
                                        pair<int,int> pos = pEnv->getRandomPosition();
                                        if(pEnv->getLocalAgentsNum(pos) < MAXNUMAGENT)/*local number of agents must be lower than threshold*/
                                        {
                                                pEnv->setLocalAgentsNum(pos);

                                                WordAgent wa(BCells[i].getID(), pEnv,simu,pos, BCELL,1);
                                                map<int,double> m = BCells[i].getDomReceptor();
                                                wa.setDomReceptor(m);
                                                vector<int> v = BCells[i].getRecReceptor();
                                                wa.setRecReceptor(v);
                                                BCells.push_back(wa);
                                                flag = false;
                                        }
                                }
                        }
		}
		cout<<".";
	}
	return true;
}

bool Trainer::initSentenceID()
{
        pEnv->setSentenceID(0);
        return true;
}

void Trainer::testSub()
{
        pEnv->testSub(20);
}

bool Trainer::saveFeatureWeights()
{
        fstream out("./result/fweight",ios::out|ios::app);
        if(out)
        {
                vector<double> fw = pModel->getFeatureWeight();
                feaSet.push_back(fw);
                for(size_t i = 0; i < fw.size();i++)
                {
                        out<<fw[i]<<"\t";
                }
                out<<endl;

        }
        out.close();

}

void Trainer::initSentences()
{
       vSen.clear();
}



