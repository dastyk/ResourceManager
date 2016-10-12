namespace Arfer
{
    partial class Arfer
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Arfer));
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.newPackageToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.existingPackageToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem1 = new System.Windows.Forms.ToolStripSeparator();
            this.saveToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveAsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem2 = new System.Windows.Forms.ToolStripSeparator();
            this.exitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.treeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.expandAllToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.collapseAllToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.itemTree = new System.Windows.Forms.TreeView();
            this.itemTreeRCCM = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.addFolderToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.itemTreeNodeRCCM = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.addFileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.addFolderToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem3 = new System.Windows.Forms.ToolStripSeparator();
            this.renameToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.removeToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem4 = new System.Windows.Forms.ToolStripSeparator();
            this.expandAllToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.collapseAllToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.itemTreeFileNodeRCCM = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.renameToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.removeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.nodeInfoBox = new System.Windows.Forms.GroupBox();
            this.nodeExt = new System.Windows.Forms.Label();
            this.nodeComp = new System.Windows.Forms.Label();
            this.nodeSize = new System.Windows.Forms.Label();
            this.menuStrip1.SuspendLayout();
            this.itemTreeNodeRCCM.SuspendLayout();
            this.itemTreeFileNodeRCCM.SuspendLayout();
            this.nodeInfoBox.SuspendLayout();
            this.SuspendLayout();
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.newPackageToolStripMenuItem,
            this.existingPackageToolStripMenuItem,
            this.toolStripMenuItem1,
            this.saveToolStripMenuItem,
            this.saveAsToolStripMenuItem,
            this.toolStripMenuItem2,
            this.exitToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(37, 24);
            this.fileToolStripMenuItem.Text = "File";
            // 
            // newPackageToolStripMenuItem
            // 
            this.newPackageToolStripMenuItem.Name = "newPackageToolStripMenuItem";
            this.newPackageToolStripMenuItem.Size = new System.Drawing.Size(147, 22);
            this.newPackageToolStripMenuItem.Text = "New Package";
            this.newPackageToolStripMenuItem.Click += new System.EventHandler(this.newPackageToolStripMenuItem_Click);
            // 
            // existingPackageToolStripMenuItem
            // 
            this.existingPackageToolStripMenuItem.Name = "existingPackageToolStripMenuItem";
            this.existingPackageToolStripMenuItem.Size = new System.Drawing.Size(147, 22);
            this.existingPackageToolStripMenuItem.Text = "Load Package";
            this.existingPackageToolStripMenuItem.Click += new System.EventHandler(this.existingPackageToolStripMenuItem_Click);
            // 
            // toolStripMenuItem1
            // 
            this.toolStripMenuItem1.Name = "toolStripMenuItem1";
            this.toolStripMenuItem1.Size = new System.Drawing.Size(144, 6);
            // 
            // saveToolStripMenuItem
            // 
            this.saveToolStripMenuItem.Enabled = false;
            this.saveToolStripMenuItem.Name = "saveToolStripMenuItem";
            this.saveToolStripMenuItem.Size = new System.Drawing.Size(147, 22);
            this.saveToolStripMenuItem.Text = "Save";
            this.saveToolStripMenuItem.Click += new System.EventHandler(this.saveToolStripMenuItem_Click);
            // 
            // saveAsToolStripMenuItem
            // 
            this.saveAsToolStripMenuItem.Enabled = false;
            this.saveAsToolStripMenuItem.Name = "saveAsToolStripMenuItem";
            this.saveAsToolStripMenuItem.Size = new System.Drawing.Size(147, 22);
            this.saveAsToolStripMenuItem.Text = "Save As";
            this.saveAsToolStripMenuItem.Click += new System.EventHandler(this.saveAsToolStripMenuItem_Click);
            // 
            // toolStripMenuItem2
            // 
            this.toolStripMenuItem2.Name = "toolStripMenuItem2";
            this.toolStripMenuItem2.Size = new System.Drawing.Size(144, 6);
            // 
            // exitToolStripMenuItem
            // 
            this.exitToolStripMenuItem.Name = "exitToolStripMenuItem";
            this.exitToolStripMenuItem.Size = new System.Drawing.Size(147, 22);
            this.exitToolStripMenuItem.Text = "Exit";
            this.exitToolStripMenuItem.Click += new System.EventHandler(this.exitToolStripMenuItem_Click);
            // 
            // menuStrip1
            // 
            this.menuStrip1.BackColor = System.Drawing.SystemColors.ControlLightLight;
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.treeToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Padding = new System.Windows.Forms.Padding(0);
            this.menuStrip1.Size = new System.Drawing.Size(613, 24);
            this.menuStrip1.TabIndex = 0;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // treeToolStripMenuItem
            // 
            this.treeToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.expandAllToolStripMenuItem,
            this.collapseAllToolStripMenuItem});
            this.treeToolStripMenuItem.Name = "treeToolStripMenuItem";
            this.treeToolStripMenuItem.Size = new System.Drawing.Size(41, 24);
            this.treeToolStripMenuItem.Text = "Tree";
            // 
            // expandAllToolStripMenuItem
            // 
            this.expandAllToolStripMenuItem.Name = "expandAllToolStripMenuItem";
            this.expandAllToolStripMenuItem.Size = new System.Drawing.Size(136, 22);
            this.expandAllToolStripMenuItem.Text = "Expand All";
            this.expandAllToolStripMenuItem.Click += new System.EventHandler(this.expandAllToolStripMenuItem_Click);
            // 
            // collapseAllToolStripMenuItem
            // 
            this.collapseAllToolStripMenuItem.Name = "collapseAllToolStripMenuItem";
            this.collapseAllToolStripMenuItem.Size = new System.Drawing.Size(136, 22);
            this.collapseAllToolStripMenuItem.Text = "Collapse All";
            this.collapseAllToolStripMenuItem.Click += new System.EventHandler(this.collapseAllToolStripMenuItem_Click);
            // 
            // itemTree
            // 
            this.itemTree.ContextMenuStrip = this.itemTreeRCCM;
            this.itemTree.Dock = System.Windows.Forms.DockStyle.Left;
            this.itemTree.Location = new System.Drawing.Point(0, 24);
            this.itemTree.Name = "itemTree";
            this.itemTree.Size = new System.Drawing.Size(298, 536);
            this.itemTree.TabIndex = 1;
            this.itemTree.AfterLabelEdit += new System.Windows.Forms.NodeLabelEditEventHandler(this.itemTree_AfterLabelEdit);
            this.itemTree.NodeMouseClick += new System.Windows.Forms.TreeNodeMouseClickEventHandler(this.itemTree_NodeMouseClick);
            // 
            // itemTreeRCCM
            // 
            this.itemTreeRCCM.Name = "itemTreeRCCM";
            this.itemTreeRCCM.Size = new System.Drawing.Size(61, 4);
            // 
            // addFolderToolStripMenuItem
            // 
            this.addFolderToolStripMenuItem.Name = "addFolderToolStripMenuItem";
            this.addFolderToolStripMenuItem.Size = new System.Drawing.Size(32, 19);
            // 
            // itemTreeNodeRCCM
            // 
            this.itemTreeNodeRCCM.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.addFileToolStripMenuItem,
            this.addFolderToolStripMenuItem1,
            this.toolStripMenuItem3,
            this.renameToolStripMenuItem,
            this.removeToolStripMenuItem1,
            this.toolStripMenuItem4,
            this.expandAllToolStripMenuItem1,
            this.collapseAllToolStripMenuItem1});
            this.itemTreeNodeRCCM.Name = "itemTreeNodeRCCM";
            this.itemTreeNodeRCCM.Size = new System.Drawing.Size(137, 148);
            // 
            // addFileToolStripMenuItem
            // 
            this.addFileToolStripMenuItem.Name = "addFileToolStripMenuItem";
            this.addFileToolStripMenuItem.Size = new System.Drawing.Size(136, 22);
            this.addFileToolStripMenuItem.Text = "Add File";
            this.addFileToolStripMenuItem.Click += new System.EventHandler(this.addFileToolStripMenuItem_Click);
            // 
            // addFolderToolStripMenuItem1
            // 
            this.addFolderToolStripMenuItem1.Name = "addFolderToolStripMenuItem1";
            this.addFolderToolStripMenuItem1.Size = new System.Drawing.Size(136, 22);
            this.addFolderToolStripMenuItem1.Text = "Add Folder";
            this.addFolderToolStripMenuItem1.Click += new System.EventHandler(this.addFolderToolStripMenuItem1_Click);
            // 
            // toolStripMenuItem3
            // 
            this.toolStripMenuItem3.Name = "toolStripMenuItem3";
            this.toolStripMenuItem3.Size = new System.Drawing.Size(133, 6);
            // 
            // renameToolStripMenuItem
            // 
            this.renameToolStripMenuItem.Name = "renameToolStripMenuItem";
            this.renameToolStripMenuItem.Size = new System.Drawing.Size(136, 22);
            this.renameToolStripMenuItem.Text = "Rename";
            this.renameToolStripMenuItem.Click += new System.EventHandler(this.renameToolStripMenuItem_Click);
            // 
            // removeToolStripMenuItem1
            // 
            this.removeToolStripMenuItem1.Name = "removeToolStripMenuItem1";
            this.removeToolStripMenuItem1.Size = new System.Drawing.Size(136, 22);
            this.removeToolStripMenuItem1.Text = "Remove";
            this.removeToolStripMenuItem1.Click += new System.EventHandler(this.removeToolStripMenuItem1_Click);
            // 
            // toolStripMenuItem4
            // 
            this.toolStripMenuItem4.Name = "toolStripMenuItem4";
            this.toolStripMenuItem4.Size = new System.Drawing.Size(133, 6);
            // 
            // expandAllToolStripMenuItem1
            // 
            this.expandAllToolStripMenuItem1.Name = "expandAllToolStripMenuItem1";
            this.expandAllToolStripMenuItem1.Size = new System.Drawing.Size(136, 22);
            this.expandAllToolStripMenuItem1.Text = "Expand All";
            this.expandAllToolStripMenuItem1.Click += new System.EventHandler(this.expandAllToolStripMenuItem1_Click);
            // 
            // collapseAllToolStripMenuItem1
            // 
            this.collapseAllToolStripMenuItem1.Name = "collapseAllToolStripMenuItem1";
            this.collapseAllToolStripMenuItem1.Size = new System.Drawing.Size(136, 22);
            this.collapseAllToolStripMenuItem1.Text = "Collapse All";
            this.collapseAllToolStripMenuItem1.Click += new System.EventHandler(this.collapseAllToolStripMenuItem1_Click);
            // 
            // itemTreeFileNodeRCCM
            // 
            this.itemTreeFileNodeRCCM.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.renameToolStripMenuItem1,
            this.removeToolStripMenuItem});
            this.itemTreeFileNodeRCCM.Name = "itemTreeFileNodeRCCM";
            this.itemTreeFileNodeRCCM.Size = new System.Drawing.Size(118, 48);
            // 
            // renameToolStripMenuItem1
            // 
            this.renameToolStripMenuItem1.Name = "renameToolStripMenuItem1";
            this.renameToolStripMenuItem1.Size = new System.Drawing.Size(117, 22);
            this.renameToolStripMenuItem1.Text = "Rename";
            this.renameToolStripMenuItem1.Click += new System.EventHandler(this.renameToolStripMenuItem1_Click);
            // 
            // removeToolStripMenuItem
            // 
            this.removeToolStripMenuItem.Name = "removeToolStripMenuItem";
            this.removeToolStripMenuItem.Size = new System.Drawing.Size(117, 22);
            this.removeToolStripMenuItem.Text = "Remove";
            this.removeToolStripMenuItem.Click += new System.EventHandler(this.removeToolStripMenuItem_Click);
            // 
            // nodeInfoBox
            // 
            this.nodeInfoBox.Controls.Add(this.nodeExt);
            this.nodeInfoBox.Controls.Add(this.nodeComp);
            this.nodeInfoBox.Controls.Add(this.nodeSize);
            this.nodeInfoBox.Location = new System.Drawing.Point(310, 31);
            this.nodeInfoBox.Name = "nodeInfoBox";
            this.nodeInfoBox.Size = new System.Drawing.Size(290, 521);
            this.nodeInfoBox.TabIndex = 3;
            this.nodeInfoBox.TabStop = false;
            // 
            // nodeExt
            // 
            this.nodeExt.AutoSize = true;
            this.nodeExt.Location = new System.Drawing.Point(6, 88);
            this.nodeExt.Name = "nodeExt";
            this.nodeExt.Size = new System.Drawing.Size(0, 13);
            this.nodeExt.TabIndex = 2;
            // 
            // nodeComp
            // 
            this.nodeComp.AutoSize = true;
            this.nodeComp.Location = new System.Drawing.Point(6, 66);
            this.nodeComp.Name = "nodeComp";
            this.nodeComp.Size = new System.Drawing.Size(0, 13);
            this.nodeComp.TabIndex = 1;
            // 
            // nodeSize
            // 
            this.nodeSize.AutoSize = true;
            this.nodeSize.Location = new System.Drawing.Point(6, 42);
            this.nodeSize.Name = "nodeSize";
            this.nodeSize.Size = new System.Drawing.Size(0, 13);
            this.nodeSize.TabIndex = 0;
            // 
            // Arfer
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.Control;
            this.ClientSize = new System.Drawing.Size(613, 560);
            this.Controls.Add(this.nodeInfoBox);
            this.Controls.Add(this.itemTree);
            this.Controls.Add(this.menuStrip1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MainMenuStrip = this.menuStrip1;
            this.MaximizeBox = false;
            this.Name = "Arfer";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Arfer";
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.itemTreeNodeRCCM.ResumeLayout(false);
            this.itemTreeFileNodeRCCM.ResumeLayout(false);
            this.nodeInfoBox.ResumeLayout(false);
            this.nodeInfoBox.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem exitToolStripMenuItem;
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.TreeView itemTree;
        private System.Windows.Forms.ContextMenuStrip itemTreeRCCM;
        private System.Windows.Forms.ToolStripMenuItem addFolderToolStripMenuItem;
        private System.Windows.Forms.ContextMenuStrip itemTreeNodeRCCM;
        private System.Windows.Forms.ToolStripMenuItem renameToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem addFileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem existingPackageToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem1;
        private System.Windows.Forms.ToolStripMenuItem newPackageToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem saveToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem saveAsToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem2;
        private System.Windows.Forms.ToolStripMenuItem addFolderToolStripMenuItem1;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem3;
        private System.Windows.Forms.ContextMenuStrip itemTreeFileNodeRCCM;
        private System.Windows.Forms.ToolStripMenuItem renameToolStripMenuItem1;
        private System.Windows.Forms.ToolStripMenuItem removeToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem removeToolStripMenuItem1;
        private System.Windows.Forms.ToolStripMenuItem treeToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem expandAllToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem collapseAllToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem4;
        private System.Windows.Forms.ToolStripMenuItem expandAllToolStripMenuItem1;
        private System.Windows.Forms.ToolStripMenuItem collapseAllToolStripMenuItem1;
        private System.Windows.Forms.GroupBox nodeInfoBox;
        private System.Windows.Forms.Label nodeSize;
        private System.Windows.Forms.Label nodeExt;
        private System.Windows.Forms.Label nodeComp;
    }
}

