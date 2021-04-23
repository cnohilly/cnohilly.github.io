package listeners;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JComboBox;

import code.*;
import mysql.first.MySQLAccess;

public class CurrentGametypeSelectionListener implements ActionListener {
	
	private GUI _gui;
	private MySQLAccess _mysql;
	
	public CurrentGametypeSelectionListener(GUI gui, MySQLAccess mysql){
		_gui = gui;
		_mysql = mysql;
	}
	public void actionPerformed(ActionEvent e) {
		@SuppressWarnings("unchecked")
		JComboBox<String> cb = (JComboBox<String>)e.getSource();
		String gametype = (String)cb.getSelectedItem();
		try {
			_gui.setCurrentGametype(gametype, _mysql.getMapsList(gametype));
			_gui.setCurrentMapSelectionBox(_mysql.getMapsList(gametype));
		} catch (Exception exc) {
		}
	}
	
}
