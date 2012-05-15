
#include <vector>
#include <iostream>

#include <cassert>
#include <ctime>
#include <cstdlib>
#include <math.h>
#include <iomanip>

#include "Parameter.hpp"
#include "WordAgent.hpp"

using namespace std;



WordAgent::WordAgent(int id, Environment * environment,
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

	_mapStatusToBehavior();
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
	bool hasRun = false;
        if(orders.size())
        {
                int now = orders.front();
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
	return hasRun;
}

bool WordAgent::_doMove()
{
        //cout<<"mo ";
        /*updating receptor before moving*/
        if(status != ACTIVE)
        {
                _mapStatusToBehavior();
                return false;
        }
	_updateSelf();
	static const int dx[] = {0, 1, 1, 1, 0, -1, -1, -1};
	static const int dy[] = {1, 1, 0, -1, -1, -1, 0, 1};
	//cout<<"&";
	int min = env->agentCount(position);
	//cout<<"*";
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

	srand(time(NULL));
	int p = rand() % pos.size();
	pair<int, int> oldPos = position;
	env->delPWordAgent(*this);
	if(category == BCELL)
	{
                env->setGrid(false,oldPos);
	}
	position = pos[p];

	env->addPWordAgent(*this);
	if(category == BCELL)
	{
                env->setGrid(true,position);
	}
	/*updating receptor after moving*/
	_updateSelf();

	return true;
}

pair<int, int> WordAgent::getPosition() const
{
	return position;
}

bool WordAgent::_mutate()
{

	if(getStatus() == MATCH)
	{
	        int a;
	        //cout<<"mu ";

		tmpFeature = domFeature;
		//cout<<"feature size is "<<tmpFeature.size()<<" ag feature size "<<agFeature.size()<<endl;
		//cin>>a;
		bool flag = false;
		/*multi-point mutation*/
		//int n = -1;
		double d = 0.0;
		double mutatedProb = 0.0;
		srand(time(NULL));
		for(size_t i = 0; i < agFeature.size(); i++)
		{
		    /*producting mutated probability*/
		    int r = rand()%10000;

		    mutatedProb = (double)r/10000.0;
		    //mutatedProb = 0.0;

		    if(mutatedProb < MUTATEPRO)
		    {

               		 int n = rand()%2;
               		 if(n == 0)
               		 {
                   		 /*increase*/
                   		 //cout<<"in"<<" ";
                   		 d = domFeature[agFeature[i]] + DETA;
                   		 //if(d < 1.0)
                   		 {
                       			 tmpFeature[agFeature[i]] = d;
					 mutatePosition.push_back(agFeature[i]);
                       			 flag = true;
                   		 }
               		 }
               		 else
               		 {
                   		 /*decrease*/
                   		 //cout<<"de"<<" ";
                   		 d = domFeature[agFeature[i]] - DETA;
                   		 //if(d > 0.0)
                   		 {
                       			 tmpFeature[agFeature[i]] = d;
					 mutatePosition.push_back(agFeature[i]);
                       			 flag = true;
                   		 }
               		 }

               		 d = 0.0;
		    }
		}

		/*calculating difference of affinity after mutation*/
		mutatedAffinity = _calMutatedAffinity(agFeature);
		int diff = int((mutatedAffinity - agAffinity) * PRECISION);
		//cout<<"differ is "<<diff<<endl;

		if(diff > 0)
		{
			setStatus(MUTATE);
		}
	}

	_mapStatusToBehavior();

	return true;
}

double WordAgent::_calAffinity(std::vector<int> receptor, int & matchSize)
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

	if(status != ACTIVE)
	{
	        //_mapStatusToBehavior();
		return false;
	}
	/*updating receptor before interacting*/
	_updateSelf();
	//cout<<"in ";
	/*gain near agents*/
	mutatePosition.clear();
	std::vector< WordAgent > nearAgents;

	if(env->getNearbyAgents(AgentID, position, nearAgents))
	{
		/*selecting objected interacted randomly*/
		for(size_t i = 0; i < nearAgents.size(); i++)
		{

			if((nearAgents[i].getCategory() == ANTIGEN) && (category == BCELL))
			{
				/*interacting between Antigens and B cells*/
				if(nearAgents[i].getStatus() == ACTIVE)
				{
				        isInteractedWithAntigen = true;
					this->agFeature = nearAgents[i].getRecReceptor();
					//cout<<"ag feature size "<<agFeature.size()<<endl;
					int matchSize = 0;
					agAffinity = _calAffinity(this->agFeature,matchSize);
					if((matchSize == (int)agFeature.size()) && (matchSize > 0))
					{
					        AGID = nearAgents[i].getID();
					        setStatus(MATCH);
					        setSentence(env->getSentence(),env->getSentenceID());
					        env->setWordAgentStatus(DIE,position,nearAgents[i].getAgentID());
					        //nearAgents[i].setStatus(DIE);

					        //cout<<nearAgents[i].getAgentID()<<" ";
					        env->decreaseAntigenNum(AGID);
					        //cout<<"ag id is "<<nearAgents[i].getID()<<", status is "<<nearAgents[i].getStatus()<<" B id is "<<ID<<endl;
						break;
                                        }
				}
			}
			else if((nearAgents[i].getCategory() == BCELL) && (category == ANTIGEN))
			{
			        if(nearAgents[i].getStatus() == ACTIVE)
				{
				        isInteractedWithAntigen = true;
					this->agFeature = nearAgents[i].getRecReceptor();
					//cout<<"ag feature size "<<agFeature.size()<<endl;
					int matchSize = 0;
					agAffinity = _calAffinity(this->agFeature,matchSize);
					if((matchSize == (int)agFeature.size()) && (matchSize > 0))
					{
					        setStatus(DIE);
					        //cout<<"antigen is die ";
					        //nearAgents[i].setAGID(ID);
					        env->setAntigenID(ID,position,nearAgents[i].getAgentID());
					        //cout<<"ag id is "<<ID<<" , status is "<<status<<" B id is "<<nearAgents[i].getID()<<endl;
					        //nearAgents[i].setStatus(MATCH);
					        env->setWordAgentStatus(MATCH,position,nearAgents[i].getAgentID());
					        //nearAgents[i].setSentence(env->getSentence(),env->getSentenceID());
					        env->setWordAgentSentence(env->getSentence(),env->getSentenceID(),position,nearAgents[i].getAgentID());
					        env->decreaseAntigenNum(ID);
						break;
                                        }
				}
                        }
		}
	}

	_mapStatusToBehavior();

	return true;
}

bool WordAgent::_clone()
{
	/*cloning by idiotype immune network and adaptive immune as equa:
	N = alpha*concentration + stimulus - suppression
	*/

	if(getStatus() == MATURE)
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

			/*cloning*/
			for(int i = 0; i < N; i++)
			{
				/*new agent by cloning: status is MEMORY*/
				/*WordAgent wa(ID,env,position,BCELL,1);
				wa.setDomReceptor(domFeature);
				wa.setRecReceptor(recFeature);
				*/

				/*add agent to environment*/
				env->addPWordAgent(*this);
				env->increaseBcellNum();
				env->setGrid(true,position);
			}
			//env->addCloneAgents(N);

			//orders.push(REGULATING);
			/*vector<set<WordAgent *> > agents = env->getAgents();
			int num = 0;
			cout<<"clone id is "<<endl;
			for(size_t j = 0 ; j < agents.size(); j++)
			{
			        for(set<WordAgent *>::iterator it = agents[j].begin(); it != agents[j].end(); it++)
			        {
			                cout<<(*it)->getID()<<" ";
                                }
                        }
                        cout<<endl;
			//cout<<"agent num is "<<num<<" after clone."<<endl;
			cin>>a;
			*/


		}
		setStatus(REGULATE);
	}
	//cout<<"clone ";
	_mapStatusToBehavior();

	return true;
}

bool WordAgent::_regulate()
{

	/*regulating by idiotype immune network of B cells as equa:
	N = concentration + stimulus - suppression
	*/
	//cout<<"regulating ";

	if(category == BCELL)
	{
                //cout<<"re ";
                if(!_getRegulation())
                {
                        std::cerr<<"Gain regulation failed!"<<std::endl;
                }


                int con = _calConcentration();
                int N = con * (int)(1.0 + stimulus - suppression);
                int num = env->getAntigenNum() + env->getBcellNum();
                //cout<<"agent num is "<<num<<" before regulation."<<endl;
                if(N > 0)
                {
                        /*cloning (stimulus)*/
                        for(int i = 0; i < N; i++)
                        {
                                /*new agent by cloning: status is ACTIVE*/
                                /*static WordAgent wa(ID,env,position,BCELL,1);
                                wa.setDomReceptor(domFeature);
				wa.setRecReceptor(recFeature);

                                /*add agent to environment*/
                                env->addPWordAgent(*this);
				env->increaseBcellNum();
				env->setGrid(true,position);

                        }
                        //env->addCloneAgents(N);
                        /*vector<set<WordAgent *> > agents = env->getAgents();
			int num = 0;
			cout<<"regulating id is "<<endl;
			for(size_t j = 0 ; j < agents.size(); j++)
			{
			        for(set<WordAgent *>::iterator it = agents[j].begin(); it != agents[j].end(); it++)
			        {
			                cout<<(*it)->getID()<<" ";
                                }
                        }
                        cout<<endl;
                        */

			//cout<<"agent num is "<<num<<" after clone."<<endl;
                }
                else if(N < 0)
                {
                        std::vector<WordAgent> nearAgents;
                        int M = -N;
                        if(env->getNearbyAgents(AgentID,position,nearAgents))
                        {
                                for(size_t i = 0; i < nearAgents.size(); i++)
                                {
                                        if(nearAgents[i].getID() == ID)
                                        {
                                                env->delPWordAgent(*this);
                                                env->decreaseBcellNum();
                                                env->setGrid(false,position);
                                                M--;
                                                if(M == 0)
                                                {
                                                        break;
                                                }
                                        }
                                }
                        }
                }
                num = env->getAntigenNum() + env->getBcellNum();
                /*int a;
                cin>>a;
                cout<<"agent num is "<<num<<" after regulation."<<endl;
                */
                setStatus(ACTIVE);
                //cout<<"regulated finished! ";
                _mapStatusToBehavior();
	}

	return true;
}

std::vector<int> WordAgent::getRecReceptor() const
{
	return recFeature;
}

bool WordAgent::_select()
{

	if(status == MUTATE)
	{
	        //cout<<"se ";
	        //cout<<"select... ";
		/*selection by competition among mutated agents:
		(1) agents generating negative feedbacks of depedency parsing are died;
		(2) agents with lower affinities are died when competing;
		*/
		/*gain neighbours*/
		std::vector<WordAgent> nearAgents;
		if(env->getNearbyAgents(AgentID, position, nearAgents))
		{
			for(size_t i = 0; i < nearAgents.size(); i++)
			{
				if((nearAgents[i].getStatus() == MUTATE) && (AGID == nearAgents[i].getAGID()))
				{
					/*comparing feedbacks*/
                                        /*if(_cmpFeedback(feedback,nearAgents[i]->getFeedback()))*/
					if(mutatedAffinity < nearAgents[i].getMutatedAffinity())
					{
						setStatus(ACTIVE);
					}
					else
					{
						nearAgents[i].setStatus(ACTIVE);
					}

				}
			}
               		if(status == MUTATE)
               		{
               		        if(_calFeedback())
               		        {
                                        domFeature = tmpFeature;
                                        env->updateFeatureWeights(domFeature);
                                        setStatus(MATURE);
                                        //int a;
                                        //cin>>a;
               		        }
               		        else
               		        {
               		                setStatus(MATCH);
                                }
                        }
		}
	}
	//cout<<"selecting ";
	_mapStatusToBehavior();

	return true;
}

bool WordAgent::_calFeedback()
{
	/*calculating feedback based on mutated receptors*/
	feedback = env->gainFeedback(this,sen);
	if(feedback.first > 0)
	{
	        cout<<"ag number is "<<env->getAntigenNum()<<endl;
	        //env->setFeedback(true);
	        if((senID == env->getSentenceID()) && (feedback.second > ACCURACYTHRESHOLD))
	        {
                        env->setFeedbackFlag(true);
	        }
	        cout<<"sentence "<<senID<<" positive feedback! ";

                return true;
	}
	//cout<<"negative feedback ";
	return false;
}

double WordAgent::getAgAffinity()
{
	return agAffinity;
}

bool WordAgent::_die()
{
        if(env->delPWordAgent(*this))
        {
                return true;
        }
	return false;
}

double WordAgent::_calSuppressByBcell(std::map<int, double> receptor)
{
	double sum = 0.0;
	map<int, double>::iterator it;
	for(size_t i = 0; i < recFeature.size(); i++)
	{
		it = receptor.find(recFeature[i]);
		if(it != receptor.end())
		{
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

bool WordAgent::_updateSelf()
{
	/*updating information of self receptor including:
	domFeature
	*/
	int fid = -1;
	vector<double> fWeight = env->getFeatureWeights();
	for(map<int,double>::iterator it = domFeature.begin(); it != domFeature.end(); it++)
	{
	        fid = (*it).first;
	        if(fWeight[fid] != (*it).second)
	        {
	                (*it).second = fWeight[fid];
                }
        }

	return true;
}



int WordAgent::getCategory()
{
	return this->category;
}


bool WordAgent::_getRegulation()
{
	stimulus = 0.0;
	suppression = 0.0;
	/*get near agents*/
	std::vector<WordAgent> nearAgents;
	if(env->getNearbyAgents(AgentID, position, nearAgents))
	{
	        /*cout<<"number of agents is "<<nearAgents.size()<<endl;
	        vector<set<WordAgent *> > agents = env->getAgents();
	        for(size_t  j = 0; j < agents.size(); j++)
	        {
	                cout<<agents[j].size()<<" ";
                }
                cout<<endl;
                */

		double affinity = 0.0;
		for(size_t i = 0; i < nearAgents.size(); i++)
		{
		        //cout<<"ID "<<nearAgents[i]->getID()<<endl;
		        /*if(nearAgents[i].getID() > 1000)
		        {
		                cout<<&nearAgents[i]<<" "<<nearAgents[i].getID()<<endl;

		                int a;
		                cin>>a;
                        }
                        */
			if((nearAgents[i].getStatus() != DIE) && (nearAgents[i].getCategory() == BCELL))
			{

				affinity = _calStimulusByBcell(nearAgents[i].getRecReceptor());
				stimulus += affinity;
				//cout<<"stimulus is "<<stimulus<<endl;

				affinity = _calSuppressByBcell(nearAgents[i].getDomReceptor());
				suppression += affinity;
				//cout<<"suppression is "<<suppression<<endl;
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
				if(ID == nearAgents[i].getID())
				{
					concentration++;
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
