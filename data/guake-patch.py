    
    @dbus.service.method(DBUS_NAME, in_signature='i', out_signature='s')
    def get_gtktab_name(self, tab_index=0):
        return self.guake.tabs.get_children()[tab_index].get_label()
        