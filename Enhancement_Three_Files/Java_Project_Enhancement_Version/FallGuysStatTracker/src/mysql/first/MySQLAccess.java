package mysql.first;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import code.GUI;

public class MySQLAccess {

	private Connection connect = null;
	private Statement statement = null;
	private PreparedStatement preparedStatement = null;
	private ResultSet resultSet = null;
	
	public MySQLAccess() throws Exception {
		try {
			Class.forName("com.mysql.cj.jdbc.Driver");
		} catch (Exception e) {
			throw e;
		}
	}
	
	public void open() throws Exception {
		try {
		connect = DriverManager.getConnection("jdbc:mysql://localhost/fall_guys?" + "user=chris&password=noh1");
		statement = connect.createStatement();
		resultSet = null;
		preparedStatement = null;
		} catch (Exception e) {
			throw e;
		}
	}
	public void readDataBase() throws Exception {
		try {
			open();
			
			resultSet = statement.executeQuery("select * from fall_guys.fallguys_maps");
			writeResultSet(resultSet);
			
			preparedStatement = connect.prepareStatement("SELECT NAME, WIN, LOSS FROM fall_guys.fallguys_maps");
			resultSet = preparedStatement.executeQuery();
			writeResultSet(resultSet);
			
			resultSet = statement.executeQuery("select * from fall_guys.fallguys_maps");
			writeMetaData(resultSet);
			
			resultSet = statement.executeQuery("select * from fall_guys.fallguys_teamwl");
			writeMetaData(resultSet);
			
		} catch (Exception e) {
			throw e;
		} finally {
			close();
		}
	}
	
	private void writeMetaData(ResultSet resultSet) throws SQLException {
		System.out.println("The columns in the table are: ");
		System.out.println("Table: " + resultSet.getMetaData().getTableName(1));
		for (int i = 1; i <= resultSet.getMetaData().getColumnCount(); i++) {
			System.out.println("Column " + i + " " + resultSet.getMetaData().getColumnName(i));
		}
	}
	
	private void writeResultSet(ResultSet resultSet) throws SQLException {
		while (resultSet.next()) {
			String mapName = resultSet.getString("NAME");
			int mapWins = resultSet.getInt("WIN");
			int mapLosses = resultSet.getInt("LOSS");
			System.out.println(mapName + " - " + mapWins + " - " + mapLosses);
		}
	}
	
	private void close() {
		try { 
			if (resultSet != null) {
				resultSet.close();
			}
			
			if (statement != null) {
				statement.close();
			}
			
			if (connect != null) {
				connect.close();
			} 
		} catch (Exception e) {
				
		}	
	}	
	
	/* 
	 * 
	 */
	public void getGametypeStats(String gameType) throws SQLException, Exception {
		open();
		if (gameType.equals("Team")) {
			preparedStatement = connect.prepareStatement("select * "
				+ "from fall_guys.fallguys_maps JOIN fall_guys.fallguys_teamwl "
				+ "ON fall_guys.fallguys_maps.map_id = fall_guys.fallguys_teamwl.map_id;");
		}
		else {
			preparedStatement = connect.prepareStatement("select * from fall_guys.fallguys_maps where TYPE = ? ;");
			preparedStatement.setString(1, gameType);
		}
		resultSet = preparedStatement.executeQuery();
		String mapName, temp;
		int mapWins, mapLosses, redWin, redLoss, blueWin, blueLoss, greenWin, greenLoss, yellowWin, yellowLoss;
		while (resultSet.next()) {
			mapName = resultSet.getString("NAME");
			mapWins = resultSet.getInt("WIN");
			mapLosses = resultSet.getInt("LOSS");
			
			temp = mapName;
			for (int x = 0; x < 20 - mapName.length(); x++) {
				temp += " ";
			}
			temp += " - " + mapWins + " - " + mapLosses;
			
			if (gameType.equals("Team")) {
				redWin = resultSet.getInt("R_WIN");
				redLoss = resultSet.getInt("R_LOSS");
				blueWin = resultSet.getInt("B_WIN");
				blueLoss = resultSet.getInt("B_LOSS"); 
				greenWin = resultSet.getInt("G_WIN");
				greenLoss = resultSet.getInt("G_LOSS");
				yellowWin = resultSet.getInt("Y_WIN");
				yellowLoss = resultSet.getInt("Y_LOSS");
				if (gameType.equals("Team")) {
					temp += " - " + redWin + " - " + redLoss + " - " + blueWin + " - " + blueLoss;
					temp += " - " + greenWin + " - " + greenLoss + " - " + yellowWin + " - " + yellowLoss;
				}
			}
			
			System.out.println(temp);
		}
		close();
	}
	
	public void updateDisplayedMaps(GUI gui)  throws SQLException, Exception {
		if (gui.getGametypeDisplayed().equals("Team")) {
			updateTeamRecord(gui);
		}
		open();
		preparedStatement = connect.prepareStatement("select * from fall_guys.fallguys_maps where TYPE = ? ;");
		preparedStatement.setString(1, gui.getGametypeDisplayed());
		resultSet = preparedStatement.executeQuery();
		String maps = "", name = "";
		int wins = 0, losses = 0;
		maps = "Map - " + gui.getGametypeDisplayed();
		maps += gui.emptySpaceString(30 - maps.length()) + "|  W  |  L  |";
		maps += "\n" + gui.dashString(maps.length()) + "\n";
		while(resultSet.next()) {
			name = resultSet.getString("NAME");
			wins = resultSet.getInt("WIN");
			losses = resultSet.getInt("LOSS");
			maps += name + gui.emptySpaceString(30 - name.length()) + "| " + gui.formatNumber(wins) + " | " + gui.formatNumber(losses) + " |\n";
		}
		gui.setDisplayedGametypeRecord(maps);
		close();
	}
	
	// Pull data for Team gametype to update the TextAreas in the GUI
	public void updateTeamRecord(GUI gui) throws SQLException, Exception {
		open();
		preparedStatement = connect.prepareStatement("select * "
				+ "from fall_guys.fallguys_maps JOIN fall_guys.fallguys_teamwl "
				+ "ON fall_guys.fallguys_maps.map_id = fall_guys.fallguys_teamwl.map_id;");
		resultSet = preparedStatement.executeQuery();
		String redString = "", blueString = "", greenString = "", yellowString = "";
		redString = " R W | R L |";
		redString += '\n' + gui.dashString(redString.length()) + '\n';
		blueString = " B W | B L |";
		blueString += '\n' + gui.dashString(blueString.length()) + '\n';
		greenString = " G W | G L |";
		greenString += '\n' + gui.dashString(greenString.length()) + '\n';
		yellowString = " Y W | Y L |";
		yellowString += '\n' + gui.dashString(yellowString.length()) + '\n';
		
		while(resultSet.next()) {
			redString += " " + gui.formatNumber(resultSet.getInt("R_WIN")) + " | " + gui.formatNumber(resultSet.getInt("R_LOSS")) + " |\n";
			blueString += " " + gui.formatNumber(resultSet.getInt("B_WIN")) + " | " + gui.formatNumber(resultSet.getInt("B_LOSS")) + " |\n";
			greenString += " " + gui.formatNumber(resultSet.getInt("G_WIN")) + " | " + gui.formatNumber(resultSet.getInt("G_LOSS")) + " |\n";
			yellowString += " " + gui.formatNumber(resultSet.getInt("Y_WIN")) + " | " + gui.formatNumber(resultSet.getInt("Y_LOSS")) + " |\n";
		}
		gui.setTeamRecord(redString,blueString,greenString,yellowString);
		close();
	}
	
	public String[] getMapTypeList() throws SQLException, Exception {
		open();
		preparedStatement = connect.prepareStatement("SELECT DISTINCT TYPE from fallguys_maps;");
		resultSet = preparedStatement.executeQuery();
		int rowCount = 0;
		while (resultSet.next()) {
			rowCount++;
		}
		String[] mapTypes = new String[rowCount + 1];
		
		preparedStatement = connect.prepareStatement("SELECT DISTINCT TYPE from fallguys_maps;");
		resultSet = preparedStatement.executeQuery();
		mapTypes[0] = "Select Gametype";
		while (resultSet.next()) {
			mapTypes[rowCount] = resultSet.getString("TYPE");
			rowCount--;
		}
		close();
		return mapTypes;
	}
	
	public String[] getMapsList(String gametype) throws SQLException, Exception {
		open();
		preparedStatement = connect.prepareStatement("SELECT NAME from fallguys_maps WHERE TYPE = ?;");
		preparedStatement.setString(1,gametype);
		resultSet = preparedStatement.executeQuery();
		int rowCount = 0;
		while (resultSet.next()) {
			rowCount++;
		}
		String[] mapNames = new String[rowCount];
		preparedStatement = connect.prepareStatement("SELECT NAME from fallguys_maps WHERE TYPE = ?;");
		preparedStatement.setString(1,gametype);
		resultSet = preparedStatement.executeQuery();
		rowCount--;
		while (resultSet.next()) {
			mapNames[rowCount] = resultSet.getString("NAME");
			rowCount--;
		}
		close();
		return mapNames;
	}
	
	public void endGame(GUI gui, int result) throws SQLException, Exception {
		open();
		int currentValue = 0;
		int mapValue = 0;
		String resultString = "";
		switch (result) {
		case 0: resultString = "LOSS";
			break;
		case 1: resultString = "WIN";
			break;
		}
		String teamLetter = "";
		if (gui.getCurrentGametype().equals("Team")) {
			switch (gui.getCurrentTeamColor()) {
			case "Red": teamLetter = "R";
				break;
			case "Blue": teamLetter = "B";
				break;
			case "Green": teamLetter = "G";
				break;
			case "Yellow": teamLetter = "Y";
				break;
			default: gui.setResultOutput("No team was selected. Result was not recorded.");
				return;
			}
		}
		preparedStatement = connect.prepareStatement("SELECT map_id," + resultString + " FROM fallguys_maps WHERE NAME = ? ;");
		preparedStatement.setString(1,gui.getCurrentMap());
		resultSet = preparedStatement.executeQuery();
		if (resultSet.next()) {
			mapValue = resultSet.getInt("map_id");
			currentValue = resultSet.getInt(resultString);
		} else {
			gui.setResultOutput("No map selected. Result was not recorded.");
			return;
		}
		preparedStatement = connect.prepareStatement("UPDATE fallguys_maps SET " + resultString + " = ? WHERE NAME = ?;");
		preparedStatement.setInt(1,currentValue+1);
		preparedStatement.setString(2,gui.getCurrentMap());
		preparedStatement.executeUpdate();
		if (gui.getCurrentGametype().equals("Team")) {
			preparedStatement = connect.prepareStatement("SELECT " + teamLetter +"_"+ resultString+ " FROM fallguys_teamwl WHERE map_id = ?;");
			preparedStatement.setInt(1,mapValue);
			resultSet = preparedStatement.executeQuery();
			if (resultSet.next()) {
				currentValue = resultSet.getInt(teamLetter + "_" + resultString);
			} else {
				gui.setResultOutput("Map not selected. Result was not recorded.");
				return;
			}
			preparedStatement = connect.prepareStatement("UPDATE fallguys_teamwl SET " + teamLetter + "_" + resultString + " = ? WHERE map_id = ?;");
			preparedStatement.setInt(1, currentValue+1);
			preparedStatement.setInt(2,mapValue);
			preparedStatement.executeUpdate();
		}
		close();
		String output = "Result recorded: " + resultString + " on " + gui.getCurrentMap();
		if (gui.getCurrentGametype().equals("Team")) {
			output += " for " + gui.getCurrentTeamColor() + " team";
		}
		gui.setResultOutput(output);
		if (gui.getCurrentGametype().equals(gui.getGametypeDisplayed())) {
			updateDisplayedMaps(gui);
		}
	}
	
}


