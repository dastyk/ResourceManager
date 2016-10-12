using System;
using System.Collections;
using System.Windows.Forms;
using System.IO;
using System.Runtime.Serialization.Formatters.Binary;

namespace jhTreeViewTools
{
	/// <summary>
	/// jhTreeViewTools is a namespace for classes around the TreeView control
	/// This (only) one is a class for saving and loading the TreeView 
	/// </summary>
	public class LoadAndSave
	{

		#region Save (saveTree, saveNode)
		/// <summary>
		/// Save the TreeView content
		/// </summary>
		/// <param name="tree"></param>
		/// <param name="filename"></param>
		/// <returns>Errorcode as int</returns>
		public static int saveTree(TreeView tree, string filename)
		{
			// Neues Array anlegen
			ArrayList al = new ArrayList();
			foreach (TreeNode tn in tree.Nodes)
			{
				// jede RootNode im TreeView sichern ...
				al.Add(tn);
			}

			// Datei anlegen
			Stream file = File.Open(filename, FileMode.Create);
			// Binär-Formatierer init.
			BinaryFormatter bf = new BinaryFormatter();
			try
			{
				// Serialisieren des Arrays
				bf.Serialize(file, al);
			}
			catch (System.Runtime.Serialization.SerializationException e)
			{
				MessageBox.Show("Serialization failed : {0}", e.Message);
				return -1; // ERROR
			}

			// Datei schliessen
			file.Close();

			return 0; // OKAY
		}
		#endregion

	
		#region Load (loadTree, searchNode)
		/// <summary>
		/// Load the TreeView content
		/// </summary>
		/// <param name="tree"></param>
		/// <param name="filename"></param>
		/// <returns>Errorcode as int</returns>
		public static int loadTree(TreeView tree, string filename)
		{
			if (File.Exists(filename))
			{
				// Datei öffnen
				Stream file = File.Open(filename, FileMode.Open);
				// Binär-Formatierer init.
				BinaryFormatter bf = new BinaryFormatter();
				// Object var. init.
				object obj = null;
				try
				{
					// Daten aus der Datei deserialisieren
					obj = bf.Deserialize(file);
				}
				catch (System.Runtime.Serialization.SerializationException e)
				{
					MessageBox.Show("De-Serialization failed : {0}", e.Message);
					return -1;
				}
				// Datei schliessen
				file.Close();

				// Neues Array erstellen
				ArrayList nodeList = obj as ArrayList;

				// load Root-Nodes
				foreach (TreeNode node in nodeList)
				{
					tree.Nodes.Add(node);
				}
				return 0;

			}
			else return -2; // File existiert nicht
		}
		
		#endregion
	}
}