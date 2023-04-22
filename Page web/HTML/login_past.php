<?php
	// On démarre la session AVANT d'écrire du code HTML
	session_start();
	
	if (isset($_POST["mdp"])){
		// On crée la variables du mot de passe dans $_SESSION
		$_SESSION['pw']  = $_POST["mdp"];
	}

?>


<!DOCTYPE html>
<html lang="fr">
<head>
	<meta charset="utf-8" />
	<title>Bluepeace</title>
	<link rel="stylesheet" href="../CSS/login-past.css" />
	<link rel="icon" href="../sources/logo-m.ico" />
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
		
		<div class="request">
		<form method="GET">
			Rechercher
			<input type="text" name="t">
			dans
			<select name="filtre">
			  <option value="nom">Nom</option>
			  <option value="prenom">Prénom</option>
			  <option value="orga">Organisation</option>
			  <option value="date_">Date d'arrivée</option>
			  <option value="time_">Heure d'arrivée</option>
			</select>
			Trier par
			<select name="sorting_mode">
			  <option value="nom">Nom</option>
			  <option value="prenom">Prénom</option>
			  <option value="orga">Organisation</option>
			  <option value="date_">Date d'arrivée</option>
			  <option value="time_">Heure d'arrivée</option>
			</select>
			<input type="submit" value="GO">
      </form>
      </div>
    <?php
			
			$servername = "inf-mysql.univ-rouen.fr"; /* Info MySQL a modifier pour le lieux d'execution */
			$username = "dourlma2";
			$password = "04012000";
			$dbname = "dourlma22";
		if (isset($_GET["sorting_mode"])) {
          $sorting_mode = $_GET["sorting_mode"];
		} else {
          $sorting_mode = "nom";
        }
        if (isset($_GET["t"]) && isset($_GET["filtre"])) {
          $t = $_GET["t"];
          $filtre = $_GET["filtre"];
        }
        
        
		if ($_SESSION['pw']=="010101"){
			
			$conn = new mysqli($servername, $username, $password, $dbname);

			if ($conn->connect_error) {
				die("Connection failed: " . $conn->connect_error);
			}
			if (isset($t) && isset($filtre)) {
			  $sql = "SELECT * FROM inscription WHERE $filtre LIKE '%$t%' ORDER BY $sorting_mode";
			} else {
			  $sql = "SELECT * FROM inscription";
			}
			$result = $conn->query($sql);
			if ($result->num_rows > 0) {
				echo "<table>";
				echo "<tr><th>Prénom</th><th>Nom</th><th>Organisation</th><th>Heure</th><th>Date</th></tr>";
				while($row = $result->fetch_assoc()) {
						echo "<tr><td>".$row["prenom"]."</td>"."<td>".$row["nom"]."</td>"."<td>".$row["orga"]."</td>"."<td>".$row["time_"]."</td>"."<td>".$row["date_"]."</td></tr>";
				}
			} else {
				echo "0 resultat";
			}
			$conn->close();
		} else {
			echo "erreur de mot de passe";
		}
	?>

</body>
</html>
