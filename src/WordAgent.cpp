
#include <vector>
#include <iostream>

#include <cassert>
#include <ctime>
#include <cstdlib>
#include <math.h>
#include <iomanip>

#include <algorithm>

#include "Parameter.hpp"
#include "WordAgent.hpp"

using namespace std;



WordAgent::WordAgent(int id, Environment * environment,Simulator * simulator,
		const pair<int, int> & pos, int cat, int con){
	ID = id;
	env = environment;
	position = pos;

	category = cat;
	concentration = con;
	status = ACTIVE;

	stimulus = 0.0;
	suppression = 0.0;

	agAffinity = 0.0;
	isInteractedWithAntigen = false;

	AgentID = 0;
	simu = simulator;

	mapStatusToBehavior();
}

bool WordAgent::addDomFeature(const vector<int> & feature)
{
	for(size_t i = 0; i < feature.size(); i++){

		domFeature.insert(make_pair(feature[i],0.0));
	}
	return true;
}

bool WordAgent::addRecFeature(const vector<int> & feature)
{
	for(size_t i = 0; i < feature.size(); i++){
		recFeature.push_back(feature[i]);
	}
	return true;
}

int WordAgent::getID()
{
	return ID;
}

bool WordAgent::run()
{
        //cout<<"agent id "<<AgentID<<" ";
        //cout<<"run ";
	bool hasRun = false;
	//cout<<"size "<<orders.size();
        if(orders.size())
        {
                int now = orders.front();
                //cout<<"now is "<<now<<" ";
                orders.pop();
                switch(now)
                {
                        case MOVING:
                                _doMove();
                                break;
                        case INTERACTING:
                                _interact();
                                break;
                        case MUTATING:
                                _mutate();
                                break;
                        case SELECTING:
                                _select();
                                break;
                        case CLONING:
                                _clone();
                                break;
                        case REGULATING:
                                _regulate();
                                break;
                        case DYING:
                                _die();
                                break;
                        default:
                                assert(0);
                }
                hasRun = true;
        }
        //cout<<"orun "<<endl;
	return hasRun;
}

bool WordAgent::_doMove()
{
        //cout<<"mo";
        /*updating receptor*/
        if(status != ACTIVE)
        {
                _mapStatusToBehavior();
                return false;
        }
	//updateSelf();
	//simu->initLocalAgents(this);
	static const int dx[] = {0, 1, 1, 1, 0, -1, -1, -1};
	static const int dy[] = {1, 1, 0, -1, -1, -1, 0, 1};
	int min = env->agentCount(position);
	vector<pair<int, int> > pos;
	for(int k = 0; k < 8; k++){
		int x = position.first + dx[k];
		int y = position.second + dy[k];
		pair<int, int> newPos = make_pair(x, y);
		if(env->xInRange(x) && env->yInRange(y)){
			if(env->agentCount(newPos) < min){
				min = env->agentCount(newPos);
				pos.clear();
			}
			if(env->agentCount(newPos) == min){
				pos.push_back(newPos);
			}
		}
	}

	orders.push(INTERACTING);
	if(min == env->agentCount(position)){
		return false;
	}

        pair<int, int> oldPos = position;
	srand(time(NULL));
	int p = rand() % pos.size();
	position = pos[p];

	simu->addWordAgent(*this);
	//resetLocalAgents();
	position = oldPos;
	//cout<<"move";
	simu->deleteWordAgent(*this);
	//cout<<"move over";

	return true;
}

pair<int, int> WordAgent::getPosition() const
{
	return position;
}

bool WordAgent::_mutate()
{
        //cout<<"mu ";
	if(status == MATCH)
	{
		tmpFeature = domFeature;
		bool flag = false;
		/*multi-point mutation*/
		double mutatedProb = 0.0;
		srand(time(NULL));
		mutatePosition.clear();
		for(size_t i = 0; i < agFeature.size(); i++)
		{
		    /*producting mutated probability*/
		    int r = rand()%1000;
		    mutatedProb = (double)r/1000.0;
		    if(mutatedProb < MUTATEPRO)
		    {
		            tmpFeature[agFeature[i]] = domFeature[agFeature[i]] + DETA;;
		            mutatePosition.push_back(agFeature[i]);
		    }
		}

		/*calculating difference of affinity after mutation*/
		mutatedAffinity = _calMutatedAffinity(agFeature);
		int matchSize = 0;
		agAffinity = _calAffinity(domFeature,matchSize);
		int diff = int((mutatedAffinity - agAffinity) * PRECISION);
		//cout<<"differ is "<<diff<<endl;

		if(diff > 0)
		{
			setStatus(MUTATE);
		}
		else
		{
		        setStatus(MATCH);
                }
	}

	mapStatusToBehavior();

	return true;
}

double WordAgent::_calAffinity(std::vector<int> & receptor, int & matchSize)
{
	/*calculating affinity*/
	matchSize = 0;
	double sum = 0.0;
        std::map<int, double>::iterator it;
        for(size_t i = 0; i < receptor.size(); i++)
        {
                it = domFeature.find(receptor[i]);
                if(it != domFeature.end())
                {
                        matchSize++;
                        sum += domFeature[receptor[i]];
                }
        }
	return sum;
}

double WordAgent::_calAffinity(std::map<int,double> & domReceptor, int & matchSize)
{
        /*calculating affinity*/
	matchSize = 0;
	double sum = 0.0;
        std::map<int, double>::iterator it;
        for(size_t i = 0; i < recFeature.size(); i++)
        {
                it = domReceptor.find(recFeature[i]);
                if(it != domReceptor.end())
                {
                        matchSize++;
                        sum += domReceptor[recFeature[i]];
                }
        }
	return sum;

}

double WordAgent::calAffinity(const std::vector<int> & receptor, std::map<int,double> & domF,int & matchSize)
{
        /*calculating affinity*/
	matchSize = 0;
	double sum = 0.0;
        std::map<int, double>::iterator it;
        for(size_t i = 0; i < receptor.size(); i++)
        {
                it = domF.find(receptor[i]);
                if(it != domF.end())
                {
                        matchSize++;
                        sum += domF[receptor[i]];
                }
        }
	return sum;
}

double WordAgent::_calStimulusByBcell(std::vector<int> receptor)
{
	double sum = 0.0;
	/*cout<<"calculating stimulus "<<endl;
	cout<<"size of receptor is "<<receptor.size()<<endl;
	for(map<int,double>::iterator it = domFeature.begin();it != domFeature.end(); it++)
	{
	        cout<<(*it).first<<" ";
        }
        cout<<endl;
        for(size_t n = 0; n < receptor.size(); n++)
        {
                cout<<receptor[n]<<" ";
        }
        cout<<endl;
        */
	if(status != DIE)
	{
		std::map<int, double>::iterator it;
		for(size_t i = 0; i < receptor.size(); i++)
		{
			it = domFeature.find(receptor[i]);
			if(it != domFeature.end())
			{
				sum += domFeature[receptor[i]];
			}
		}
	}

	return sum;
}

double WordAgent::_calMutatedAffinity(std::vector<int> receptor)
{
	/*calculating affinity after mutation*/
	double sum = 0.0;
	std::map<int, double>::iterator it;

	for(size_t i = 0; i < receptor.size(); i++)
	{
		it = tmpFeature.find(receptor[i]);
		if(it != tmpFeature.end())
		{
			sum += tmpFeature[receptor[i]];
		}
	}


	return sum;
}

int WordAgent::getStatus()
{
	return status;
}

void WordAgent::setStatus(int s)
{
	status = s;
}

bool WordAgent::_interact()
{
	/*interaction between word-agents:
	(1) Antigens and B cells;
	(2) B cells
	*/
	//cout<<"in ";
	updateSelf();
	if(status != ACTIVE)
	{
		return false;
	}
	simu->interact(*this);
	//cout<<"oin";
	return true;
}

bool WordAgent::_clone()
{
	/*cloning by idiotype immune network and adaptive immune as equa:
	N = alpha*concentration + stimulus - suppression
	*/
	//cout<<"co ";
	if(status == MATURE)
	{
	        //cout<<"cl ";
	        //cout<<"id is "<<ID<<endl;
	        int a;
		if(!_getRegulation())
		{
			std::cerr<<"Gain regulation failed!"<<std::endl;
		}
		double alpha = 1.0 + agAffinity;
		int con = _calConcentration();
		int N = (int)(alpha + stimulus - suppression) * con;
		//cout<<"clone number is "<<N<<endl;
		//cin>>a;

		if(N > 0)
		{
		        //vector<WordAgent> v;
		        //int agentNum = env->getAntigenNum() + env->getBcellNum();
		        //cout<<"agentNum is "<<agentNum<<endl;
		        //cout<<"clone ";
			/*cloning*/
			srand((unsigned)time(NULL));
			for(int i = 0; i < N; i++)
			{
			        simu->addWordAgent(*this);
			}

		}
		setStatus(REGULATE);
	}
	mapStatusToBehavior();

	return true;
}

bool WordAgent::_regulate()
{

	/*regulating by idiotype immune network of B cells as equa:
	N = concentration + stimulus - suppression
	*/
	//cout<<"regulating ";
	//cout<<"re ";
	if((category == BCELL))
	{
                //cout<<"re ";
                if(!_getRegulation())
                {
                        std::cerr<<"Gain regulation failed!"<<std::endl;
                }


                int con = _calConcentration();
                int N = con * (int)(1.0 + stimulus - suppression);
                //int num = env->getAntigenNum() + env->getBcellNum();
                //cout<<"agent num is "<<num<<" before regulation."<<endl;
                if(N > 0)
                {
                        /*cloning (stimulus)*/
                        for(int i = 0; i < N; i++)
                        {
                                /*new agent by cloning: status is ACTIVE*/
                                simu->addWordAgent(*this);
                        }

                }
                else if(N < 0)
                {
                        //cout<<"reg delete ";
                        std::vector<WordAgent> nearAgents;
                        int M = -N;
                        simu->regulateByDelete(*this,M);
                }

                setStatus(ACTIVE);
                mapStatusToBehavior();
	}

	return true;
}

std::vector<int> WordAgent::getRecReceptor() const
{
	return recFeature;
}

bool WordAgent::_select()
{
        //cout<<"se ";
	simu->select(*this);
	return true;
}

bool WordAgent::_calFeedback(WordAgent & wordAgent)
{
	/*calculating feedback based on mutated receptors*/
	feedback = env->gainFeedback(&wordAgent,wordAgent.getSentence(),wordAgent.getFather());
	if(feedback.first > 0)
	{
	       // cout<<"**ag number is "<<env->getAntigenNum()<<endl;
	        //env->setFeedback(true);
	        if((wordAgent.getSentenceID() == env->getSentenceID()) && (feedback.second > ACCURACYTHRESHOLD))
	        {
                        env->setFeedbackFlag(true);

	        }
	        //cout<<"sentence "<<wordAgent.getSentenceID()<<" positive feedback by memory or antibody! ";


                return true;
	}
	//cout<<"negative feedback ";
	return false;
}

double WordAgent::getAgAffinity()
{
	return agAffinity;
}

bool WordAgent::setAffinity(double affinity)
{
        agAffinity = affinity;
        return true;
}

bool WordAgent::_die()
{
	//cout<<"die";
        if(simu->deleteWordAgent(*this))
        {
		//cout<<"die over";
                return true;
        }
	return false;
}

double WordAgent::_calSuppressByBcell(std::map<int, double> & receptor)
{

	double sum = 0.0;
	map<int, double>::iterator it;
	for(size_t i = 0; i < recFeature.size(); i++)
	{
		it = receptor.find(recFeature[i]);
		if(it != receptor.end())
		{
		        //cout<<"suppress ";
		        //cout<<receptor[recFeature[i]];
			sum += receptor[recFeature[i]];
		}
	}

	return sum;
}

std::map<int, double> WordAgent::getDomReceptor()
{
	return domFeature;
}

std::map<int, double> WordAgent::getTmpReceptor()
{
	return tmpFeature;
}

bool WordAgent::updateSelf()
{
	/*updating information of self receptor including:
	domFeature
	*/
	if(category == BCELL)
	{
                int fid = -1;
                vector<double> fWeight = env->getFeatureWeights();
                for(map<int,double>::iterator it = domFeature.begin(); it != domFeature.end(); it++)
                {
                        fid = it->first;
                        if(fWeight[fid] != it->second)
                        {
                                it->second = fWeight[fid];
                        }
                }
	}

	return true;
}



int WordAgent::getCategory()
{
	return category;
}

void WordAgent::setCategory(int cat)
{
        category = cat;
}


bool WordAgent::_getRegulation()
{
	stimulus = 0.0;
	suppression = 0.0;
	/*get near agents*/
	std::vector<WordAgent> nearAgents;
	if(env->getNearbyAgents(AgentID, position, nearAgents))
	{
		double affinity = 0.0;
		for(size_t i = 0; i < nearAgents.size(); i++)
		{
			if(nearAgents[i].getStatus() != DIE)
			{
			        //if((nearAgents[i].getCategory() == MEMORYBCELL) || (nearAgents[i].getCategory() == BCELL))
			        if(nearAgents[i].getCategory() == BCELL)
			        {
                                        affinity = _calStimulusByBcell(nearAgents[i].getRecReceptor());
                                        stimulus += affinity;
                                        //cout<<"stimulus is "<<stimulus<<endl;
                                        map<int, double> dom = nearAgents[i].getDomReceptor();
                                        affinity = _calSuppressByBcell(dom);
                                        suppression += affinity;
                                        //cout<<"suppression is "<<suppression<<endl;
			        }
			}
		}
	}
	return true;
}

double WordAgent::getMutatedAffinity()
{
	return mutatedAffinity;
}

int WordAgent::getConcentration()
{
	return _calConcentration();
}

int WordAgent::_calConcentration()
{
        //cout<<"calcu concen ";
	concentration = 1;
	vector<WordAgent> nearAgents;
	if(env->getNearbyAgents(AgentID, position, nearAgents))
	{
		for(size_t i = 0; i < nearAgents.size(); i++)
		{
			if(nearAgents[i].getStatus() != DIE)
			{
			        if((nearAgents[i].getCategory() == BCELL ) ||
                                        (nearAgents[i].getCategory() == MEMORYBCELL ))
                                {
                                        if(ID == nearAgents[i].getID())
                                        {
                                                concentration++;
                                        }
                                }
			}
		}

	}
	//cout<<"finish calcu ";
	return concentration;
}

void WordAgent::updateConcentration()
{
	/*gain neighbour*/
	int sum = 0;
	vector<WordAgent > nearAgents;
	if(env->getNearbyAgents(AgentID, position, nearAgents))
	{
		for(size_t i = 0; i < nearAgents.size(); i++)
		{
			if(nearAgents[i].getStatus() != DIE)
			{
				if(ID == nearAgents[i].getID())
				{
					sum++;
				}
			}
		}
	}
	concentration = sum;
}

void WordAgent::_mapStatusToBehavior()
{
	if(category == ANTIGEN)
	{
		/*selecting behavior according to status of antigen*/
		switch(status)
		{
			case ACTIVE:
				orders.push(MOVING);
				break;
			case DIE:
				orders.push(DYING);
				break;
			default:
				assert(0);
		}
	}
	else
	{
		/*selecting behavior according to status of B cell*/
		switch(status)
		{
			case ACTIVE:
                                orders.push(MOVING);
				break;
			case MATCH:
				orders.push(MUTATING);
				break;
			case MUTATE:
				orders.push(SELECTING);
				break;
			case MATURE:
				orders.push(CLONING);
				break;
			case DIE:
				orders.push(DYING);
				break;
                        case REGULATE:
				orders.push(REGULATING);
				break;
			default:
				assert(0);
		}
	}
}

void WordAgent::setPosition(std::pair<int,int> p)
{
        position.first = p.first;
        position.second = p.second;
}

bool WordAgent::setDomReceptor(std::map<int, double> & rec)
{
        for(map<int, double>::iterator it = rec.begin(); it != rec.end(); it++)
        {
                domFeature.insert((*it));
        }
        return true;
}

bool WordAgent::setRecReceptor(std::vector<int> & rec)
{
        for(size_t i = 0; i < rec.size(); i++)
        {
                recFeature.push_back(rec[i]);
        }
        return true;
}

int WordAgent::getAGID()
{
        return AGID;
}

bool WordAgent::setAGID(int id)
{
        AGID = id;
        return true;
}

bool WordAgent::setAgentID(const int id)
{
        AgentID = id;
        return true;
}
int WordAgent::getAgentID()
{
        return AgentID;
}

Sentence WordAgent::getSentence()
{
        return sen;
}

bool WordAgent::setSentence(const Sentence & sentence, int sentenceID)
{
        sen = sentence;
        senID = sentenceID;
        return true;
}

int WordAgent::getSentenceID()
{
        return senID;
}

std::vector<int> WordAgent::getMemoryBcellReceptor()
{
        return memoryFeature;
}

bool WordAgent::setMemoryBcellReceptor(std::vector<int> & memRec)
{
        memoryFeature = memRec;
        return true;
}

std::vector<int> WordAgent::getAntibodyReceptor()
{
        return antibodyFeature;
}

bool WordAgent::setAntibodyReceptor(std::vector<int> & antibodyRec)
{
        antibodyFeature = antibodyRec;
        return true;
}

bool WordAgent::isSame(std::vector<int> & s, std::vector<int> & d)
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

bool WordAgent::_calFeedback()
{
        //cout<<endl<<"**";
        /*calculating feedback based on mutated receptors*/
	feedback = env->gainFeedback(this,sen,father);
	if(feedback.first > 0)
	{
	        cout<<"ag number is "<<env->getAntigenNum()<<endl;
	        //env->setFeedback(true);

	        if((senID == env->getSentenceID()) && (feedback.second > ACCURACYTHRESHOLD))
	        {
                        env->setFeedbackFlag(true);
	        }
	        cout<<"sentence "<<senID<<" positive feedback! "<<endl;
	        vector<map<int, WordAgent> >  agents = env->getAgents();
	        /*for(size_t i = 0; i < agents.size(); i++)
	        {
	                cout<<agents[i].size()<<" ";
                }
                cout<<endl;
                */

                return true;
	}
	//cout<<"negative feedback ";

        return false;
}

std::vector<int> WordAgent::getAgReceptor()
{
   return agFeature;
}

bool WordAgent::setAgReceptor(std::vector<int> & rec)
{
        agFeature = rec;
        return true;
}

double WordAgent::calAffinity(const std::vector<int> & agRec, int &matchSize)
{
        /*calculating affinity*/
	matchSize = 0;
	double sum = 0.0;
        std::map<int, double>::iterator it;
        for(size_t i = 0; i < agRec.size(); i++)
        {
                it = domFeature.find(agRec[i]);
                if(it != domFeature.end())
                {
                        matchSize++;
                        sum += domFeature[agRec[i]];
                }
        }
	return sum;
}
double WordAgent::calAffinity( std::map<int,double> & bRec, int & matchSize)
{
      /*calculating affinity*/
	matchSize = 0;
	double sum = 0.0;
        std::map<int, double>::iterator it;
        for(size_t i = 0; i < recFeature.size(); i++)
        {
                it = bRec.find(recFeature[i]);
                if(it != bRec.end())
                {
                        matchSize++;
                        sum += bRec[recFeature[i]];
                }
        }
	return sum;
}

void    WordAgent::mapStatusToBehavior()
{
        while(orders.size())
        {
                orders.pop();
        }
        //cout<<"size "<<orders.size()<<" ";
        if(category == ANTIGEN)
	{
		/*selecting behavior according to status of antigen*/
		switch(status)
		{
			case ACTIVE:
				orders.push(MOVING);
				break;
			case DIE:
				orders.push(DYING);
				break;
			default:
				assert(0);
		}
	}
	else
	{
		/*selecting behavior according to status of B cell*/
		switch(status)
		{
			case ACTIVE:
                                orders.push(MOVING);
				break;
			case MATCH:
				orders.push(MUTATING);
				break;
			case MUTATE:
				orders.push(SELECTING);
				break;
			case MATURE:
				orders.push(CLONING);
				break;
			case DIE:
				orders.push(DYING);
				break;
                        case REGULATE:
				orders.push(REGULATING);
				break;
			default:
				assert(0);
		}
	}
}

bool WordAgent::calFeedback()
{
        feedback = env->gainFeedback(this,sen,father);
	if(feedback.first > 0)
	{
	        //cout<<"ag number is "<<simu->getAgNum()<<endl;
	        //env->setFeedback(true);

	        if((senID == env->getSentenceID()) && (feedback.second > ACCURACYTHRESHOLD))
	        {
                        env->setFeedbackFlag(true);
	        }
	        //cout<<"sentence "<<senID<<" positive feedback! "<<endl;
	        cout<<"*";


                return true;
	}
	//cout<<"negative feedback ";

        return false;
}


std::vector<int> WordAgent::getMutatePosition()
{
        return mutatePosition;
}

void WordAgent::setFather(std::vector<int> & fa)
{
        father = fa;
}
std::vector<int> WordAgent::getFather()
{
        return father;
}

void WordAgent::printReceptor()
{
        map<int,double>::iterator it = domFeature.begin();
        for(; it != domFeature.end();it++)
        {
                if(it->second > 0.1)
                {
                        cout<<"("<<it->first<<","<<it->second<<")"<<" ";
                }
        }
        cout<<"*";
}

bool WordAgent::insertLocalAgents(int agentID)
{
        localInteractFlag.push_back(agentID);
        return true;
}

bool WordAgent::setLocalAgents(int agentID)
{
        //localInteractFlag[agentID] = true;
        return true;
}

bool WordAgent::resetLocalAgents()
{
        localInteractFlag.clear();
        return true;
}

bool WordAgent::haveInteracted(int agentID)
{
        //cout<<"h";
        if(localInteractFlag.size() == 0)
        {
                localInteractFlag.push_back(agentID);
                return false;
        }
        //cout<<agentID;

        vector<int>::iterator it = find(localInteractFlag.begin(),localInteractFlag.end(), agentID);

        if(it == localInteractFlag.end())
        {
                localInteractFlag.push_back(agentID);
                return false;
        }
        //cout<<"&h";


        return true;
}
