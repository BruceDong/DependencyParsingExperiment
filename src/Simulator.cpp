#include <vector>
#include <iostream>

#include "Simulator.hpp"
#include "Parameter.hpp"

using namespace std;

Simulator::Simulator(Environment * environment)
{
        rows = ROWS;
        cols = COLS;
        agentId = 0;
	env = environment;
	times = 1;
	resetAgents();
	agNum = 0;
}
bool Simulator::resetAgents()
{
        vWordAgents.clear();
	vWordAgents.resize(rows * cols, map<int,WordAgent>());
	return true;
}
int Simulator::_calcSub(const pair<int, int> & pos) const
{
	return pos.first * cols + pos.second;
}

bool Simulator::_isSame(const std::vector<int> & s, const std::vector<int> & d)
{
        if(s.size() != d.size())
        {
                return false;
        }

        for(size_t i = 0; i < s.size(); i++)
        {
                if(s[i] != d[i])
                {
                        return false;
                }
        }
        return true;
}

bool Simulator::addWordAgent(WordAgent & pWordAgent)
{
        if(pWordAgent.getAgentID() == 0)
        {
                if(pWordAgent.getCategory() == ANTIGEN)
                {
                        agNum++;
                }
                agentId++;
                pWordAgent.setAgentID(agentId);
                vWordAgents[_calcSub(pWordAgent.getPosition())].insert(map<int,WordAgent>::value_type(agentId,pWordAgent));
        }
        else
        {
                //cout<<pWordAgent.getAgentID()<<" ";
                vWordAgents[_calcSub(pWordAgent.getPosition())].insert(map<int,WordAgent>::value_type(pWordAgent.getAgentID(),pWordAgent));
        }
	return true;
}

bool Simulator::deleteWordAgent(WordAgent & pWordAgent)
{
        pWordAgent.setStatus(DIE);
        //vWordAgents[_calcSub(pWordAgent.getPosition())].erase(pWordAgent.getAgentID());

        return true;
}

bool Simulator::interact(WordAgent & wa)
{
        int cate = wa.getCategory();
        int posi = _calcSub(wa.getPosition());

        if(cate == BCELL)
        {
                /*interact with Antigen agents*/
                map<int,WordAgent>::iterator it = vWordAgents[posi].begin();
                while(it != vWordAgents[posi].end())
                {
                        int agentID = it->second.getAgentID();
                        /*if(wa.haveInteracted(agentID))
                        {
                               // cout<<"%"<<endl;
                                it++;
                                continue;
                        }
                        else
                        */
                        if((it->second.getCategory()== ANTIGEN) && (it->second.getStatus() == ACTIVE))
                        {


                                //cout<<"B^";
                                it->second.insertLocalAgents(agentID);
                                int matchSize = 0;
                                vector<int> ag = it->second.getRecReceptor();
                                double affinity = wa.calAffinity(ag,matchSize);
                                if((matchSize > 0) && (matchSize == ag.size()))
                                {

                                        /*set status*/
                                        wa.setStatus(MATCH);
                                        wa.setAGID(it->second.getID());
                                        it->second.setStatus(DIE);
                                        agNum--;

                                        /*mapping behavior*/
                                        it->second.mapStatusToBehavior();

                                        wa.setSentence(env->getSentence(),env->getSentenceID());
                                        vector<int> father = env->getFather();
                                        wa.setFather(father);

                                        wa.setAgReceptor(ag);
                                        break;
                                }
                                it++;
                        }
                        else
                        {
                                it++;
                        }
                        //it++;
                }
                wa.mapStatusToBehavior();
        }
        else if(cate == ANTIGEN)
        {

                /*interact with Bcell agents*/
                map<int,WordAgent>::iterator it =  vWordAgents[posi].begin();
                while(it != vWordAgents[posi].end())
                {
                        int agentID = it->second.getAgentID();
                        /*if(wa.haveInteracted(agentID))
                        {
                                //cout<<"%"<<endl;
                                it++;
                                continue;
                        }
                        else
                        */
                        if((it->second.getCategory() == BCELL)&& (it->second.getStatus()== ACTIVE))
                        {


                                //cout<<"ag^";
                                it->second.insertLocalAgents(agentID);
                                int matchSize = 0;
                                map<int,double> b = it->second.getDomReceptor();
                                double affinity = wa.calAffinity(b,matchSize);
                                vector<int> ag = wa.getRecReceptor();
                                if((matchSize > 0) && (matchSize == ag.size()))
                                {
                                        /*set status*/
                                        wa.setStatus(DIE);
                                        it->second.setStatus(MATCH);
                                        it->second.setAGID(wa.getID());
                                        agNum--;

                                        /*mapping behavior*/

                                        it->second.mapStatusToBehavior();

                                        it->second.setSentence(env->getSentence(),env->getSentenceID());
                                        it->second.setAgReceptor(ag);
                                        break;
                                }
                                it++;
                        }
                        else
                        {
                                it++;
                        }
                        //it++;
                }
                wa.mapStatusToBehavior();
        }
        return true;
}

bool Simulator::select(WordAgent & wa)
{
        if(wa.getStatus() == MUTATE)
        {
                /**/
                map<int,WordAgent>::iterator it =  vWordAgents[_calcSub(wa.getPosition())].begin();
                while(it != vWordAgents[_calcSub(wa.getPosition())].end())
                {
                        if(it->second.getAgentID() != wa.getAgentID())
                        {
                                vector<int> s = wa.getAgReceptor();
                                vector<int> d = it->second.getAgReceptor();
                                if(_isSame(s,d))
                                {
                                        if(it->second.getMutatedAffinity() < wa.getMutatedAffinity())
                                        {
                                                it->second.setStatus(MATCH);
                                                it->second.mapStatusToBehavior();
                                        }
                                        else if(it->second.getMutatedAffinity() > wa.getMutatedAffinity())
                                        {
                                                wa.setStatus(MATCH);
                                                break;
                                        }
                                }
                        }
                        it++;
                }

                if(wa.getStatus() == MUTATE)
                {
                        if(wa.calFeedback())
                        {
                                map<int,double> tmp = wa.getTmpReceptor();
                                wa.setDomReceptor(tmp);
                                wa.setStatus(MATURE);
                                env->updateFeatureWeights(tmp);
                        }
                        else
                        {
                                wa.setStatus(MATCH);
                        }
                }
                wa.mapStatusToBehavior();
        }
        return true;
}
bool Simulator::regulateByDelete(WordAgent & wa,int N)
{
        int c = N;
        int posi = _calcSub(wa.getPosition());
        map<int,WordAgent>::iterator it = vWordAgents[posi].begin();
        while(it != vWordAgents[posi].end())
        {
                if(it->second.getAgentID() != wa.getAgentID())
                {
                        if(it->second.getID() == wa.getID())
                        {
                                if(it->second.getStatus() == ACTIVE)
                                {
                                        it->second.setStatus(DIE);
                                        //it->second.mapStatusToBehavior();
                                        c--;
                                }
                        }
                }
                if(c == 0)
                {
                        break;
                }
                it++;
        }
        return true;
}
int Simulator::getAgNum()
{
        return agNum;
}

bool Simulator::run(const Sentence & sen, const std::vector<int> & fa)
{
        //cout<<"Learning from the "<<times++<<" 'th sentence..."<<endl;

        /*reset interating objects*/
	bool hasRun = true;
	int c;
	/*for(size_t i = 0; i < vWordAgents.size(); i++)
	{
	        cout<<vWordAgents[i].size()<<" ";
        }
        cout<<endl;
	for(size_t i = 0; i < vWordAgents.size(); i++)
	{
                for(map<int,WordAgent>::iterator it = vWordAgents[i].begin(); it != vWordAgents[i].end(); it++)
                {
                        if(it->second.getStatus() == ACTIVE)
                        {
                                cout<<it->second.getCategory()<<" ";

                        }
                }
                cout<<endl;
	}
	*/

	//cin>>c;
	/*termination conditions:
	(1) All antigens are killed;
	(2) All B cells are active;
	*/
	//cout<<endl;

	env->setFeedbackFlag(false);
	vector<map<int, WordAgent> > agents = vWordAgents;
	int a;
	//int round = 0;
	while(hasRun){
	        hasRun = false;

                for(size_t i = 0; i < vWordAgents.size(); i++){
			for(map<int,WordAgent>::iterator it = vWordAgents[i].begin(); it != vWordAgents[i].end(); it++)
			{
			        //cout<<"&";
			        it->second.run();

				if(!env->getFeedbackFlag())
				{
				        hasRun = true;
                                }
			}
			_release();

			//cout<<"****"<<endl;
			//cout<<"i is "<<i<<" ";
			/*release*/

		}
		//cout<<"######";

		/*if(!hasRun)
		{
		        env->removeAntigen();
		}
		*/
	}

	/*for(size_t i = 0; i < vWordAgents.size(); i++)
	{
                for(map<int,WordAgent>::iterator it = vWordAgents[i].begin(); it != vWordAgents[i].end(); it++)
                {
                        it->second.printReceptor();
                }
	}
	*/

	/*remove antigen*/
	_removeAg();

	//cout<<"Learning finished!"<<endl;

	return true;
}

bool Simulator::_release()
{
        int a = 0;
        for(size_t i = 0; i < vWordAgents.size(); i++)
        {
               for(map<int,WordAgent>::iterator it = vWordAgents[i].begin(); it != vWordAgents[i].end(); )
                {
                        if(it->second.getStatus() == DIE)
                        {
                                //cout<<"d";
                                vWordAgents[i].erase(it++);
                                a++;
                        }
                        else
                        {
                                ++it;
                        }
                }
        }
        //cout<<"del "<<a<<" ";

        return true;
}

bool Simulator::_removeAg()
{
        //cout<<endl<<"remove ag"<<endl;
        int a = 0;
        for(size_t i = 0; i < vWordAgents.size(); i++)
        {
               for(map<int,WordAgent>::iterator it = vWordAgents[i].begin(); it != vWordAgents[i].end(); )
                {
                        if(it->second.getCategory() == ANTIGEN)
                        {
                                //cout<<"d";
                                vWordAgents[i].erase(it++);
                                agNum--;
                                a++;
                        }
                        else
                        {
                                ++it;
                        }
                }
        }
        a = 0;
        for(size_t i = 0; i < vWordAgents.size(); i++)
        {
               for(map<int,WordAgent>::iterator it = vWordAgents[i].begin(); it != vWordAgents[i].end(); it++)
                {
                        if(it->second.getCategory() == ANTIGEN)
                        {
                                cout<<"ag ";
                        }
                        a++;


                }
        }
        //cout<<"number of agents is "<<a<<endl;

        //cin>>a;
        return true;
}


void Simulator::init()
{
        times = 0;
}

void Simulator::initAccuracy()
{
        historyAccuracy.clear();
}

void Simulator::recordAccuracy(double & acc)
{
        historyAccuracy.push_back(acc);
}

std::vector<double> Simulator::getAccuracy()
{
        return historyAccuracy;
}

bool Simulator::initLocalAgents(WordAgent * wa)
{

        wa->resetLocalAgents();
        int posi = _calcSub(wa->getPosition());
        map<int,WordAgent>::iterator it = vWordAgents[posi].begin();
        while(it != vWordAgents[posi].end())
        {
                int agentID = it->second.getAgentID();
                if(agentID != wa->getAgentID())
                {
                        it->second.insertLocalAgents(agentID);
                }
                it++;
        }

        return true;
}

