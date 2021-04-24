package code;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.Point;
import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JTextArea;
import javax.swing.border.TitledBorder;

import listeners.*;
import mysql.first.MySQLAccess;

public class GUI {

	private JFrame 	_frame;									// Entire display frame
	private JPanel 	_mapsPanel,								// Displaying maps and names
					_topGametypeSelectionPanel,				// Buttons for selecting which Gametype to display
					_bottomCurrentMapPanel,					// Used for selecting the current map being played and the result
					_resultOutputPanel;						// Used to displaying the result output for the selected map
	private JTextArea	_mapsTextArea,						// Text area to list and display the maps for the currently selected Gametype
						_rText,_bText,_gText,_yText,		// Text areas to list win/loss for specific colors
						_resultOutputTextArea;				// 
	private String	_gametypeRecordDisplayed = "Race",		// Used to determine which maps to display based on Gametype; Default 'Race' Gametype
					_currentGametype = "Race",				// Currently selected Gametype being played
					_currentMap = null,						// Currently selected Map being played
					_currentTeamColor = null;				// Currently selected Team Color being played, if applicable
	private JComboBox<String> 	_currentGametypeSelectionBox,	// 
								_currentMapSelectionBox,		// 
								_currentTeamSelectionBox;		// 
	
	public GUI(MySQLAccess sqlAccess) {
		_frame = new JFrame("Falls Guys Stat Tracker");
		
		// Creating the top panel with buttons to switch which gametype maps to display win/loss records for
		GridBagConstraints c = new GridBagConstraints();
		_topGametypeSelectionPanel = new JPanel();
		_topGametypeSelectionPanel.setLayout(new GridBagLayout());
		TitledBorder selectionBorder = BorderFactory.createTitledBorder(BorderFactory.createLineBorder(Color.black),"Gametype Displayed");
		selectionBorder.setTitleJustification(TitledBorder.CENTER);
		_topGametypeSelectionPanel.setBorder(selectionBorder);
		c.weightx = 1; c.weighty = 1; c.insets = new Insets(15,40,15,40);
		c.fill = GridBagConstraints.HORIZONTAL;
		c.gridx = 0;
		_topGametypeSelectionPanel.add(createSelectionButton("Race",sqlAccess),c);
		c.gridx = 1;
		_topGametypeSelectionPanel.add(createSelectionButton("Team",sqlAccess),c);
		c.gridx = 2;
		_topGametypeSelectionPanel.add(createSelectionButton("Survival",sqlAccess),c);
		c.gridx = 3;
		_topGametypeSelectionPanel.add(createSelectionButton("Final",sqlAccess),c);
		_topGametypeSelectionPanel.setPreferredSize(new Dimension(750,75));
		
		
		c = new GridBagConstraints();
		// Creating the Panel used to display maps and win/loss record, creating TextAreas used to display maps and team color records
		_mapsPanel = new JPanel();
		_mapsPanel.setLayout(new GridBagLayout());
		TitledBorder mapsBorder = BorderFactory.createTitledBorder(BorderFactory.createLineBorder(Color.black),"Game Records");
		mapsBorder.setTitleJustification(TitledBorder.CENTER);
		_mapsPanel.setBorder(mapsBorder);
		Font textAreaFont = new Font("Consolas", Font.PLAIN, 15);
		_mapsTextArea = createTextArea(textAreaFont);
		_rText = createTextArea(textAreaFont);
		_rText.setBackground(new Color(255, 94, 94));
		_bText = createTextArea(textAreaFont);
		_bText.setBackground(new Color(94, 140, 255));
		_gText = createTextArea(textAreaFont);
		_gText.setBackground(new Color(94, 255, 94));
		_yText = createTextArea(textAreaFont);
		_yText.setBackground(new Color(255, 255, 94));
		try {
			sqlAccess.updateDisplayedMaps(this);
		} catch (Exception e) {
		}
		_mapsPanel.setPreferredSize(new Dimension(750,500));
		
		c = new GridBagConstraints();
		// Creating the bottom panel for selecting which map (and team if applicable) is currently being played and selecting the end result
		_bottomCurrentMapPanel = new JPanel();
		_bottomCurrentMapPanel.setLayout(new GridBagLayout());
		TitledBorder bottomBorder = BorderFactory.createTitledBorder(BorderFactory.createLineBorder(Color.black),"Current Map Selection");
		bottomBorder.setTitleJustification(TitledBorder.CENTER);
		_bottomCurrentMapPanel.setBorder(bottomBorder);
		String[] mapTypes = {""};
		try {
			mapTypes = sqlAccess.getMapTypeList();
		} catch (Exception e) {
		}
		_currentGametypeSelectionBox = new JComboBox<>(mapTypes);
		_currentGametypeSelectionBox.addActionListener(new CurrentGametypeSelectionListener(this,sqlAccess));
		c.fill = GridBagConstraints.HORIZONTAL;
		c.insets = new Insets(20,20,20,20);
		c.gridx = 0; c.gridy = 0; c.gridheight = 2; c.weightx = 1;
		_bottomCurrentMapPanel.add(_currentGametypeSelectionBox,c);
		_currentMapSelectionBox = new JComboBox<>();
		_currentMapSelectionBox.addActionListener(new CurrentMapSelectionListener(this));
		c.gridx = 1;
		_bottomCurrentMapPanel.add(_currentMapSelectionBox,c);
		_currentTeamSelectionBox = new JComboBox<>();
		_currentTeamSelectionBox.addActionListener(new CurrentTeamSelectionListener(this));
		c.gridx = 2;
		_bottomCurrentMapPanel.add(_currentTeamSelectionBox,c);
		resetSelectionBoxes();
		JButton winButton = new JButton("Win");
		winButton.setFocusable(false);
		winButton.addActionListener(new ResultButtonListener(this,1, sqlAccess));
		c.gridx = 3; c.gridy = 0; c.gridheight = 1;
		c.insets = new Insets(5,25,5,5);
		_bottomCurrentMapPanel.add(winButton,c);
		JButton lossButton = new JButton("Loss");
		lossButton.setFocusable(false);
		lossButton.addActionListener(new ResultButtonListener(this,0, sqlAccess));
		c.gridy = 1; 
		_bottomCurrentMapPanel.add(lossButton,c);
		_bottomCurrentMapPanel.setPreferredSize(new Dimension(750,100));
		
		c = new GridBagConstraints();
		_resultOutputPanel = new JPanel();
		_resultOutputPanel.setLayout(new GridBagLayout());
		TitledBorder resultBorder = BorderFactory.createTitledBorder(BorderFactory.createLineBorder(Color.black),"Result Output");
		resultBorder.setTitleJustification(TitledBorder.CENTER);
		_resultOutputPanel.setBorder(resultBorder);
		_resultOutputTextArea = createTextArea(textAreaFont);
		c.insets = new Insets(0,0,0,0);
		c.fill = GridBagConstraints.BOTH;
		c.anchor = GridBagConstraints.FIRST_LINE_START;
		c.weightx = 1; c.weighty = 1;
		c.gridx = 0; c.gridy = 0;
		_resultOutputPanel.add(_resultOutputTextArea,c);
		_resultOutputPanel.setPreferredSize(new Dimension(750,50));
		
		c = new GridBagConstraints();
		// Adding the separate panels into the main frame
		_frame.setLayout(new GridBagLayout());
		c.gridx = 0; c.gridy = 0;
		_frame.add(_topGametypeSelectionPanel,c);
		c.gridy = 1;
		_frame.add(_mapsPanel,c);
		c.gridy = 2;
		_frame.add(_bottomCurrentMapPanel,c);
		c.gridy = 3;
		_frame.add(_resultOutputPanel,c);
		
		
		
		_frame.setVisible(true);
		_frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		pack();
		_frame.setLocationRelativeTo(null);
	}
	
	private JButton createSelectionButton(String gametype, MySQLAccess mysql) {
		JButton selectionButton = new JButton(gametype); 
		selectionButton.setFocusable(false);
		selectionButton.setMinimumSize(new Dimension(150,100));
		selectionButton.addActionListener(new GametypeSelectionButtonListener(this,gametype,mysql));
		return selectionButton;
	}
	
	private JTextArea createTextArea(Font font) {
		JTextArea textArea = new JTextArea();
		textArea.setLineWrap(false);
		textArea.setEditable(false);
		textArea.setFocusable(false);
		textArea.setFont(font);
		return textArea;
	}
	
	public void setCurrentTeamColor(String color){
		_currentTeamColor = color;
	}
	public String getCurrentTeamColor() {
		return _currentTeamColor;
	}
	
	public void setCurrentGametype(String gametype, String[] mapChoices) {
		_currentGametype = gametype;
		setCurrentMapSelectionBox(mapChoices);
		setCurrentTeamSelectionBox();
		pack();
	}
	public String getCurrentGametype() {
		return _currentGametype;
	}
	
	public void setCurrentMap(String mapName) {
		_currentMap = mapName;
	}
	public String getCurrentMap() {
		return _currentMap;
	}
	
	public void setGametypeDisplayed(String s) {
		_gametypeRecordDisplayed = s;
	}
	public String getGametypeDisplayed() {
		return _gametypeRecordDisplayed;
	}
	
	// Used to write the maps for the current gametype to the mapsTextArea and then display them
	//		and includes the team win records if the gametype is selected
	public void setDisplayedGametypeRecord(String maps) {
		_mapsTextArea.setText(maps);
		
		_mapsPanel.removeAll();
		GridBagConstraints c = new GridBagConstraints();
		c.insets = new Insets(0,0,0,0);
		c.fill = GridBagConstraints.BOTH;
		c.anchor = GridBagConstraints.FIRST_LINE_START;
		c.weightx = 2; c.weighty = 1;
		c.gridx = 0; c.gridy = 0;
		_mapsPanel.add(_mapsTextArea,c);
		c.weightx = 1;
		if (_gametypeRecordDisplayed.equals("Team")) {
			c.gridx++;
			_mapsPanel.add(_rText,c);
			c.gridx++;
			_mapsPanel.add(_bText,c);
			c.gridx++;
			_mapsPanel.add(_gText,c);
			c.gridx++;
			_mapsPanel.add(_yText,c);
		}
		
		pack();
	}
	
	public void setTeamRecord(String red, String blue, String green, String yellow) {
		_rText.setText(red);
		_bText.setText(blue);
		_gText.setText(green);
		_yText.setText(yellow);
	}
	
	public void resetSelectionBoxes() {
		_currentGametypeSelectionBox.setSelectedIndex(0);
		String[] emptyString  = {};
		setCurrentMapSelectionBox(emptyString);
		setCurrentTeamSelectionBox();
	}
	
	public void setCurrentMapSelectionBox(String[] choices) {
		_currentMapSelectionBox.removeAllItems();
		String selectMap = "Select Map";
		_currentMapSelectionBox.addItem(selectMap + emptySpaceString(30 - selectMap.length()));
		for (int x = 0; x < choices.length; x++) {
			_currentMapSelectionBox.addItem(choices[x]);
		}
		_currentMapSelectionBox.setSelectedIndex(0);
		pack();
	}
	
	public void setCurrentTeamSelectionBox() {
		_currentTeamSelectionBox.removeAllItems();
		String selectTeam = "Select Team";
		_currentTeamSelectionBox.addItem(selectTeam + emptySpaceString(20 - selectTeam.length()));
		if (_currentGametype.equals("Team")) {
			_currentTeamSelectionBox.setVisible(true);
			_currentTeamSelectionBox.addItem("Red");
			_currentTeamSelectionBox.addItem("Blue");
			_currentTeamSelectionBox.addItem("Green");
			_currentTeamSelectionBox.addItem("Yellow");
		} 
		else {
			_currentTeamSelectionBox.setVisible(false);
		}
		_currentTeamSelectionBox.setSelectedIndex(0);
		pack();
	}
	
	public void setResultOutput(String output) {
		_resultOutputTextArea.setText(output);
	}

	public void pack(){
		Point frameLocation = _frame.getLocation();
		Dimension dim = _frame.getSize();
		dim.setSize(dim.getWidth() + 1,dim.getHeight() + 1);
		_frame.setSize(dim);
		_frame.pack();
		_frame.setLocation(frameLocation);
	}
	
	public String emptySpaceString(int x) {
		String emptySpace = "";
		for (int i = 0; i < x; i++) {
			emptySpace += " ";
		}
		return emptySpace;
	}
	
	public String dashString(int x) {
		String dashes = "";
		for (int i = 0; i < x; i++) {
			dashes += "-";
		}
		return dashes;
	}
	
	public String formatNumber(int x) {
		String output = "";
		if (x > 99) {
			output = "" + x;
		}
		else if (x > 9) {
			output = " " + x;
		}
		else {
			output = " " + x + " ";
		}
		return output;
	}
}
