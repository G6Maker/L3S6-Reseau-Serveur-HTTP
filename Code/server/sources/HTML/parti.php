<!DOCTYPE html>
<html lang="fr">
	<head>
		<meta charset="utf-8" />
		<link rel="stylesheet" href="../CSS/parti.css" />
		<link rel="icon" href="../sources/logo-m.ico" />
		<title>Evènement</title>
	</head>

	<body>
		
		<div class="topima">
			<img class="topimag" src="../sources/topima.jpg" alt="bannière">
		</div>
		<div class="index">
			<a href="./event.php" style="text-decoration:none">
				<div class="event">
					<p class="even">Évènement</p>
				</div>
			</a>
			<a href="./benevoles.html" style="text-decoration:none">
				<div class="qui">
					<p class="even">Les bénévoles</p>
				</div>
			</a>
			<a href="./home.html" style="text-decoration:none"><img class="logo" src="../sources/logo-c.png" alt="Logo Bluepeace"></a>
			
			<a href="./contact.html" style="text-decoration:none">
				<div class="contact">
					<p class="even">Contact</p>
				</div>
			</a>
			<a href="./info.html" style="text-decoration:none">
				<div class="info">
					<p class="even">Information</p>
				</div>
			</a>
		</div>
		<br/>
		<br/>
<?php
	
			$servername = "inf-mysql.univ-rouen.fr"; /* Info MySQL a modifier pour le lieux d'execution */
			$username = "dourlma2";
			$password = "04012000";
			$dbname = "dourlma22";

	$conn = new mysqli($servername, $username, $password, $dbname);

	if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
	}
  $sql = "SELECT prenom, nom, orga FROM inscription";
	$result = $conn->query($sql);
  if ($result->num_rows > 0) {
    echo "<table>";
    echo "<tr><th>Prénom</th><th>Nom</th><th>Organisation</th></tr>";
    while($row = $result->fetch_assoc()) {
        echo "<tr><td>".$row["prenom"]."</td>"."<td>".$row["nom"]."</td>"."<td>".$row["orga"]."</td></tr>";
    }
	} else {
    echo "0 Personne inscrite";
	}
$conn->close();
?>



	</body>
</html>

