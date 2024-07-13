# for pulling in next set of updates from gitHub to the server

git fetch
git pull
mvn compile
mvn exec:java -Dexec.mainClass="com.cyocum.CYocumServer"

