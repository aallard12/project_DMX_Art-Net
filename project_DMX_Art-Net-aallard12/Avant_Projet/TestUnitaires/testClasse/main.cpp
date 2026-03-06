#include <QCoreApplication>
#include "interfaceaccessbddagence.h"
#include <iostream>
#include <string>

using namespace std ;

int main(int argc, char *argv[])
{
    QCoreApplication app (argc, argv);
    InterfaceAccessBddAgence client1;
    int choix, retour;
    string nom, prenom, email;
    cin>>choix;
    switch (choix) {
    case 1:
        cout<<"le nom : ";
        cin>>nom;
        cout<<"le prénom : ";
        cin>>prenom;
        cout<<"l'email : ";
        cin>>email;
        retour = client1.ajouterClient(QString::fromStdString(nom), QString::fromStdString(prenom), QString::fromStdString(email));
        cout<<retour;
        break;
    case 2:
        cout<<"le nom : ";
        cin>>nom;
        cout<<"le prénom : ";
        cin>>prenom;
        cout<<"l'email incorrect : ";
        cin>>email;
        retour = client1.ajouterClient(QString::fromStdString(nom), QString::fromStdString(prenom), QString::fromStdString(email));
        cout<<retour;
        break;
    case 3:
        cout<<"le nom (un nombre) : ";
        cin>>nom;
        cout<<"le prénom (un nombre) : ";
        cin>>prenom;
        cout<<"l'email (un nombre) : ";
        cin>>email;
        retour = client1.ajouterClient(QString::fromStdString(nom), QString::fromStdString(prenom), QString::fromStdString(email));
        cout<<retour;
        break;
    default:
        break;
    }
}
