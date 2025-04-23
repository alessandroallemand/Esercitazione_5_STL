#include "Utils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include "Eigen/Eigen"
using namespace Eigen;
using namespace std;
namespace PolygonalLibrary
{
 

bool ImportMesh(PolygonalMesh& mesh){
    if (!ImportCell0Ds(mesh))
    {
        cerr << "Errore nell'importazione di Cell0Ds all'interno della mesh" << endl;
        return false;
    }

    if (!ImportCell1Ds(mesh))
    {
        cerr << "Errore nell'importazione di Cell1Ds all'interno della mesh" << endl;
        return false;
    }

    if (!ImportCell2Ds(mesh))
    {
        cerr << "Errore nell'importazione di Cell2Ds all'interno della mesh" << endl;
        return false;
    }

    edges_length(mesh);
	area_area(mesh);
    
    return true;
}

bool ImportCell0Ds(PolygonalMesh& mesh)
{
	
	ifstream file("Cell0Ds.csv");

    if(file.fail()){
        cerr << "Errore nell'apertura del file Cell0Ds"<< endl;
        return false;
    }
	
	//salvo le righe del file in questa lista di stringhe 
	
    list<string> linesList;

    string line;
    while (getline(file, line))
        linesList.push_back(line);

    file.close();

    // rimuovo l'intestazione del file Id;Marker;X;Y 
	
    linesList.pop_front();
    
	
    mesh.NumCell0Ds = linesList.size();
    if (mesh.NumCell0Ds == 0)
    {
        cerr << "There is no cell 0D" << endl;
        return false;
    }
    

	
    mesh.Cell0DsId.reserve(mesh.NumCell0Ds);
    mesh.Cell0DsCoordinates = Eigen::MatrixXd::Zero(3, mesh.NumCell0Ds);

    for (string& line : linesList)
    {		
        istringstream lineStream(line);

        char sep;
        unsigned int id;
        unsigned int marker;
        Vector2d coord;
		
		/*salvo le coordinate per ogni punto in colonne, la cui prima riga è costituita dall' Id*/
        lineStream >>  id >> sep >>marker >> sep>> mesh.Cell0DsCoordinates(0, id) >>sep>> mesh.Cell0DsCoordinates(1, id);
        mesh.Cell0DsId.push_back(id);
	
        //mappo tutti i punti il cui marker è diverso da 0
        if(marker != 0)
		{
			auto it = mesh.Cell0DMarkers.find(marker);
			if(it != mesh.Cell0DMarkers.end())
				mesh.Cell0DMarkers[marker].push_back(id);
			else
				mesh.Cell0DMarkers.insert({marker, {id}});
		}

    }

    return true;
}


bool ImportCell1Ds(PolygonalMesh& mesh)
{

	ifstream file("Cell1Ds.csv");

    if(file.fail()){
        cerr << "Errore nell'apertura del file Cell0Ds"<< endl;
        return false;
    }
        

    list<string> linesList;
    string line;
    while (getline(file, line))
        linesList.push_back(line);

    file.close();

    linesList.pop_front();

    mesh.NumCell1Ds = linesList.size();
    if (mesh.NumCell1Ds == 0)
    {
        cerr << "There is no cell 1D" << endl;
        return false;
    }

    mesh.Cell1DsId.reserve(mesh.NumCell1Ds);
    mesh.Cell1DsExtrema = Eigen::MatrixXi(2, mesh.NumCell1Ds);

    for (string& line : linesList)
    {
        istringstream lineStream(line);

        char sep;
        unsigned int id;
        unsigned int marker;
        Vector2i vertices;
		
		/*La matrice Cell1DsExtrema avrà due righe: la prima conterrà i nodi 
        di origine dei lati, mentre la seconda i nodi di destinazione.

        Come per il caso delle coordinate, ogni colonna rappresenta un lato, indicando 
        rispettivamente il punto di partenza e quello di arrivo.*/
		
        lineStream >>  id >> sep >> marker>> sep >>  mesh.Cell1DsExtrema(0, id) >>sep>>  mesh.Cell1DsExtrema(1, id);
        mesh.Cell1DsId.push_back(id);
		if(marker != 0)
		{
			auto it = mesh.Cell1DMarkers.find(marker);
			if(it != mesh.Cell1DMarkers.end())
				mesh.Cell1DMarkers[marker].push_back(id);
			else
				mesh.Cell1DMarkers.insert({marker, {id}});
		}
        
    }
    return true;
}


bool ImportCell2Ds(PolygonalMesh& mesh)
{
	ifstream file("Cell2Ds.csv");
    if (!file.is_open())
	{
		cerr << "Errore nell'apertura del file Cell2Ds"<< endl;
        return false;
	}

    string tmp;
	getline(file, tmp);
    char sep;
	unsigned int id;
	unsigned int marker;
	unsigned int num_vertices;
	unsigned int num_edges;
	
	 while (getline(file, tmp))
    {
		istringstream lineStream(tmp);
		
		lineStream >>  id >>sep>> marker >> sep>>num_vertices>>sep;
		mesh.Cell2DsId.push_back(id);
		
		/*Creo un vettore di vertici di dimensione num_vertices e successivamente memorizzo questo vettore
        nel vettore Cell2dsVertices che contiene i vertici di tutti i poligoni.*/
		
		vector<unsigned int> Vertices(num_vertices);
		for(int i = 0; i<num_vertices; i++)
			lineStream >> Vertices[i]>>sep;
		mesh.Cell2DsVertices.push_back(Vertices);
		
		//faccio lo stesso procedimento con gli edges
		
		lineStream>>num_edges>>sep;
		vector<unsigned int> Edges(num_edges);
		for(int i = 0; i<num_edges; i++)
			lineStream >> Edges[i]>>sep;
		mesh.Cell2DsEdges.push_back(Edges);
    }
	mesh.NumCell2Ds = mesh.Cell2DsId.size();
	file.close();
    return true;
}

bool edges_length(PolygonalMesh& mesh)
{
	for(unsigned int i = 0; i<mesh.NumCell2Ds; i++)
	{
        /*con questi for, prendo ogni poligono (i), e per ogni poligono prendo ogni vertice esistente 
        appartenente al poligono (j)*/
		for(unsigned int j = 0; j<mesh.Cell2DsEdges[i].size();j++)
		{	
			
			vector<unsigned int>& edges = mesh.Cell2DsEdges[i];
			
            //prendo le coordinate dei punti estremi dell' j-esimo edge
			int& Origin_index = mesh.Cell1DsExtrema(0,edges[j]);
			int& End_index = mesh.Cell1DsExtrema(1,edges[j]);
			
			//mi ricavo le coordinate dei punti estremi del segmento, e faccio il teorema di pitagora 
            //per ricavarne la lunghezza
			double& X_Origin = mesh.Cell0DsCoordinates(0,Origin_index);
			double& Y_Origin = mesh.Cell0DsCoordinates(1,Origin_index);
			double& X_End = mesh.Cell0DsCoordinates(0,End_index);
			double& Y_End = mesh.Cell0DsCoordinates(1,End_index);	
			
			double distance = sqrt(pow(X_Origin-X_End,2)+pow(Y_Origin-Y_End,2));
			
			if(distance < 1e-16)
			{
				cout<<"il poligono con ID "<< i <<" presenta il vertice con ID "<<edges[j]<<" che è di lunghezza 0"<<endl;
				return false;
			}
		}
	}
	cout<<"nessun edge ha lunghezza 0 "<<endl;
	return true;
}


bool area_area(PolygonalMesh& mesh)
{
	
	for(unsigned int i = 0; i<mesh.NumCell2Ds; i++)
	{
		double area = 0.0;
		unsigned int n = mesh.Cell2DsVertices[i].size();
		for(unsigned int j = 0; j < n; j++)
		{
			/*per far sì che la formula per il calcolo dell'area sia funzionante, l'ultimo vertice deve 
            chiudersi con il primo e per fare ciò utilizzo le operazioni algebriche modulo n, in modo 
            tale che raggiunto l'inidce n, l'indice j ritorni direttamente a 0*/
			unsigned int& P1_id = mesh.Cell2DsVertices[i][j];
			unsigned int& P2_id = mesh.Cell2DsVertices[i][(j+1)%n];
			
			double& X_P1 = mesh.Cell0DsCoordinates(0,P1_id);
			double& Y_P1 = mesh.Cell0DsCoordinates(1,P1_id);
			double& X_P2 = mesh.Cell0DsCoordinates(0,P2_id);
			double& Y_P2 = mesh.Cell0DsCoordinates(1,P2_id);
			
			area += X_P1*Y_P2-X_P2*Y_P1;
		}
		
		area = 0.5*abs(area);
		
		if(area < 1e-12)
		{
			cout<<"C'è un errore nel poligono che ha ID "<<i<<", ha area pari a 0."<<endl;
			return false;
		}
	}
	cout<<"Non ci sono poligoni che hanno area pari a 0."<<endl;
	return true;
}
}