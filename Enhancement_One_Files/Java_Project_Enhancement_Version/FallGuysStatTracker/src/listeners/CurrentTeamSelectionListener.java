package listeners;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JComboBox;

import code.*;

public class CurrentTeamSelectionListener implements ActionListener {
	
	private GUI _gui;
	
	public CurrentTeamSelectionListener(GUI gui){
		_gui = gui;
	}
	public void actionPerformed(ActionEvent e) {
		@SuppressWarnings("unchecked")
		JComboBox<String> cb = (JComboBox<String>)e.getSource();
		String teamColor = (String)cb.getSelectedItem();
		_gui.setCurrentTeamColor(teamColor);
	}
	
}
