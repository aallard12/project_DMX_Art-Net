#!/bin/bash



# 1. On prépare les changements locaux d'abord
# Utiliser "." est plus fiable que "*" pour détecter les suppressions
git add .

# 2. On demande le message à l'utilisateur
echo "Message du commit :"
read message

# 3. On crée le commit LOCALEMENT
# Tes changements sont maintenant "ancrés" dans ton historique
git commit -m "$message"

# 4. On gère la synchronisation avec le serveur
git config pull.rebase false

# On récupère les éventuelles nouveautés sur GitHub
# Le --no-edit accepte automatiquement le message de fusion
git pull origin nhenriette --no-edit

# 5. On envoie tout (tes commits + le commit de fusion s'il y en a eu un)
git push origin nhenriette


echo "Opération terminée !"
