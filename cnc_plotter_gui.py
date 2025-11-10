#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
CNC Plotter GUI - Interfaz Gráfica para ESP32 S3 CNC
Universidad Militar - Proyecto de Comunicaciones
Autores: Julia & Team
Fecha: Noviembre 2025
"""

import tkinter as tk
from tkinter import ttk, messagebox, filedialog
import serial
import serial.tools.list_ports
import threading
import time
import json
from typing import List, Tuple
import math

class CNCPlotterGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("🎨 CNC Plotter Controller - ESP32 S3")
        self.root.geometry("1200x800")
        self.root.configure(bg='#2b2b2b')
        
        # Variables
        self.serial_port = None
        self.is_connected = False
        self.is_drawing = False
        self.drawing_points = []
        self.current_pos = [0, 0]  # Posición actual del CNC
        self.pen_is_down = False
        
        # Configuración CNC (ajustable)
        self.steps_per_mm = 51.2  # 4096 pasos por 80mm ≈ 51.2 pasos/mm
        self.canvas_width = 600
        self.canvas_height = 600
        self.work_area_width = 150  # mm (ajusta según tu CNC)
        self.work_area_height = 150  # mm
        self.scale_factor = self.canvas_width / self.work_area_width  # píxeles por mm
        
        # Colores tema oscuro
        self.bg_color = '#2b2b2b'
        self.fg_color = '#ffffff'
        self.canvas_bg = '#1e1e1e'
        self.grid_color = '#3a3a3a'
        self.draw_color = '#00ff00'
        self.preview_color = '#ffaa00'
        
        self.setup_ui()
        self.update_ports()
        
    def setup_ui(self):
        """Configurar la interfaz de usuario"""
        
        # ============================================
        # PANEL SUPERIOR - CONEXIÓN Y CONTROL
        # ============================================
        top_frame = tk.Frame(self.root, bg=self.bg_color, padx=10, pady=10)
        top_frame.pack(fill=tk.X)
        
        # Puerto Serial
        tk.Label(top_frame, text="Puerto:", bg=self.bg_color, fg=self.fg_color, font=('Arial', 10, 'bold')).pack(side=tk.LEFT, padx=5)
        
        self.port_var = tk.StringVar()
        self.port_combo = ttk.Combobox(top_frame, textvariable=self.port_var, width=15, state='readonly')
        self.port_combo.pack(side=tk.LEFT, padx=5)
        
        # Botón Actualizar Puertos
        self.btn_refresh = tk.Button(top_frame, text="🔄", command=self.update_ports, 
                                     bg='#4a4a4a', fg=self.fg_color, font=('Arial', 10, 'bold'))
        self.btn_refresh.pack(side=tk.LEFT, padx=2)
        
        # Botón Conectar
        self.btn_connect = tk.Button(top_frame, text="🔌 Conectar", command=self.toggle_connection,
                                     bg='#2d7a2d', fg=self.fg_color, font=('Arial', 10, 'bold'), width=12)
        self.btn_connect.pack(side=tk.LEFT, padx=5)
        
        # Estado de conexión
        self.lbl_status = tk.Label(top_frame, text="⭕ Desconectado", bg=self.bg_color, 
                                   fg='#ff6666', font=('Arial', 10, 'bold'))
        self.lbl_status.pack(side=tk.LEFT, padx=10)
        
        # Separador
        ttk.Separator(top_frame, orient=tk.VERTICAL).pack(side=tk.LEFT, fill=tk.Y, padx=10)
        
        # Botón Home
        self.btn_home = tk.Button(top_frame, text="🏠 Home", command=self.go_home,
                                 bg='#4a7a9d', fg=self.fg_color, font=('Arial', 10, 'bold'), width=10)
        self.btn_home.pack(side=tk.LEFT, padx=5)
        
        # Botón Calibrar
        self.btn_calibrate = tk.Button(top_frame, text="⚙️ Calibrar", command=self.show_calibration_window,
                                       bg='#9d7a4a', fg=self.fg_color, font=('Arial', 10, 'bold'), width=10)
        self.btn_calibrate.pack(side=tk.LEFT, padx=5)
        
        # ============================================
        # PANEL CENTRAL - CANVAS Y CONTROLES
        # ============================================
        center_frame = tk.Frame(self.root, bg=self.bg_color)
        center_frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
        
        # Panel Izquierdo - Canvas de Dibujo
        left_frame = tk.Frame(center_frame, bg=self.bg_color)
        left_frame.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=(0, 10))
        
        tk.Label(left_frame, text="📐 Área de Dibujo", bg=self.bg_color, fg=self.fg_color, 
                font=('Arial', 12, 'bold')).pack(pady=(0, 5))
        
        # Canvas con borde
        canvas_container = tk.Frame(left_frame, bg='#4a4a4a', padx=2, pady=2)
        canvas_container.pack()
        
        self.canvas = tk.Canvas(canvas_container, width=self.canvas_width, height=self.canvas_height,
                               bg=self.canvas_bg, cursor='pencil')
        self.canvas.pack()
        
        # Dibujar grid
        self.draw_grid()
        
        # Eventos del mouse
        self.canvas.bind('<Button-1>', self.start_draw)
        self.canvas.bind('<B1-Motion>', self.draw)
        self.canvas.bind('<ButtonRelease-1>', self.end_draw)
        
        # Información del canvas
        info_frame = tk.Frame(left_frame, bg=self.bg_color)
        info_frame.pack(pady=10)
        
        self.lbl_mouse_pos = tk.Label(info_frame, text="Mouse: (0, 0) mm", bg=self.bg_color, 
                                      fg=self.fg_color, font=('Arial', 9))
        self.lbl_mouse_pos.pack(side=tk.LEFT, padx=10)
        
        self.lbl_cnc_pos = tk.Label(info_frame, text="CNC: (0, 0) pasos", bg=self.bg_color,
                                    fg=self.fg_color, font=('Arial', 9))
        self.lbl_cnc_pos.pack(side=tk.LEFT, padx=10)
        
        self.canvas.bind('<Motion>', self.update_mouse_position)
        
        # Panel Derecho - Controles
        right_frame = tk.Frame(center_frame, bg=self.bg_color, width=300)
        right_frame.pack(side=tk.RIGHT, fill=tk.Y)
        right_frame.pack_propagate(False)
        
        tk.Label(right_frame, text="🎮 Panel de Control", bg=self.bg_color, fg=self.fg_color,
                font=('Arial', 12, 'bold')).pack(pady=(0, 10))
        
        # Botones de acción principales
        btn_frame1 = tk.Frame(right_frame, bg=self.bg_color)
        btn_frame1.pack(fill=tk.X, pady=5)
        
        self.btn_draw = tk.Button(btn_frame1, text="🎨 DIBUJAR EN CNC", command=self.send_drawing,
                                 bg='#2d7a2d', fg=self.fg_color, font=('Arial', 12, 'bold'),
                                 height=2, state=tk.DISABLED)
        self.btn_draw.pack(fill=tk.X, padx=10, pady=5)
        
        self.btn_pause = tk.Button(btn_frame1, text="⏸️ PAUSAR", command=self.pause_drawing,
                                   bg='#9d7a2d', fg=self.fg_color, font=('Arial', 11, 'bold'),
                                   height=2, state=tk.DISABLED)
        self.btn_pause.pack(fill=tk.X, padx=10, pady=5)
        
        self.btn_clear = tk.Button(btn_frame1, text="🗑️ Limpiar Canvas", command=self.clear_canvas,
                                   bg='#7a2d2d', fg=self.fg_color, font=('Arial', 11, 'bold'))
        self.btn_clear.pack(fill=tk.X, padx=10, pady=5)
        
        # Separador
        ttk.Separator(right_frame, orient=tk.HORIZONTAL).pack(fill=tk.X, pady=15)
        
        # Control del lápiz
        tk.Label(right_frame, text="✏️ Control del Lápiz", bg=self.bg_color, fg=self.fg_color,
                font=('Arial', 11, 'bold')).pack(pady=(0, 5))
        
        btn_frame2 = tk.Frame(right_frame, bg=self.bg_color)
        btn_frame2.pack(fill=tk.X, pady=5)
        
        self.btn_pen_up = tk.Button(btn_frame2, text="⬆️ Subir", command=self.pen_up,
                                    bg='#4a7a9d', fg=self.fg_color, font=('Arial', 10))
        self.btn_pen_up.pack(side=tk.LEFT, fill=tk.X, expand=True, padx=(10, 5))
        
        self.btn_pen_down = tk.Button(btn_frame2, text="⬇️ Bajar", command=self.pen_down,
                                      bg='#4a7a9d', fg=self.fg_color, font=('Arial', 10))
        self.btn_pen_down.pack(side=tk.RIGHT, fill=tk.X, expand=True, padx=(5, 10))
        
        # Separador
        ttk.Separator(right_frame, orient=tk.HORIZONTAL).pack(fill=tk.X, pady=15)
        
        # Guardar/Cargar
        tk.Label(right_frame, text="💾 Archivo", bg=self.bg_color, fg=self.fg_color,
                font=('Arial', 11, 'bold')).pack(pady=(0, 5))
        
        btn_frame3 = tk.Frame(right_frame, bg=self.bg_color)
        btn_frame3.pack(fill=tk.X, pady=5)
        
        self.btn_save = tk.Button(btn_frame3, text="💾 Guardar", command=self.save_drawing,
                                 bg='#4a4a9d', fg=self.fg_color, font=('Arial', 10))
        self.btn_save.pack(side=tk.LEFT, fill=tk.X, expand=True, padx=(10, 5))
        
        self.btn_load = tk.Button(btn_frame3, text="📂 Cargar", command=self.load_drawing,
                                 bg='#4a4a9d', fg=self.fg_color, font=('Arial', 10))
        self.btn_load.pack(side=tk.RIGHT, fill=tk.X, expand=True, padx=(5, 10))
        
        # Separador
        ttk.Separator(right_frame, orient=tk.HORIZONTAL).pack(fill=tk.X, pady=15)
        
        # Progreso
        tk.Label(right_frame, text="📊 Progreso", bg=self.bg_color, fg=self.fg_color,
                font=('Arial', 11, 'bold')).pack(pady=(0, 5))
        
        self.progress = ttk.Progressbar(right_frame, length=280, mode='determinate')
        self.progress.pack(padx=10, pady=5)
        
        self.lbl_progress = tk.Label(right_frame, text="0 / 0 puntos", bg=self.bg_color,
                                     fg=self.fg_color, font=('Arial', 9))
        self.lbl_progress.pack()
        
        # Separador
        ttk.Separator(right_frame, orient=tk.HORIZONTAL).pack(fill=tk.X, pady=15)
        
        # Configuración avanzada
        tk.Label(right_frame, text="⚙️ Configuración", bg=self.bg_color, fg=self.fg_color,
                font=('Arial', 11, 'bold')).pack(pady=(0, 5))
        
        config_frame = tk.Frame(right_frame, bg=self.bg_color)
        config_frame.pack(padx=10, fill=tk.X)
        
        tk.Label(config_frame, text="Pasos/mm:", bg=self.bg_color, fg=self.fg_color,
                font=('Arial', 9)).grid(row=0, column=0, sticky=tk.W, pady=2)
        
        self.entry_steps = tk.Entry(config_frame, width=10, font=('Arial', 9))
        self.entry_steps.insert(0, str(self.steps_per_mm))
        self.entry_steps.grid(row=0, column=1, padx=5, pady=2)
        
        self.btn_update_config = tk.Button(config_frame, text="✓", command=self.update_config,
                                          bg='#4a7a9d', fg=self.fg_color, font=('Arial', 9), width=3)
        self.btn_update_config.grid(row=0, column=2, pady=2)
        
        # ============================================
        # PANEL INFERIOR - CONSOLA
        # ============================================
        bottom_frame = tk.Frame(self.root, bg=self.bg_color, padx=10, pady=10)
        bottom_frame.pack(fill=tk.BOTH, expand=False)
        
        tk.Label(bottom_frame, text="📟 Consola Serial", bg=self.bg_color, fg=self.fg_color,
                font=('Arial', 10, 'bold')).pack(anchor=tk.W, pady=(0, 5))
        
        # Área de texto con scrollbar
        console_container = tk.Frame(bottom_frame, bg='#4a4a4a', padx=1, pady=1)
        console_container.pack(fill=tk.BOTH, expand=True)
        
        scrollbar = tk.Scrollbar(console_container)
        scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
        
        self.console = tk.Text(console_container, height=8, bg='#1e1e1e', fg='#00ff00',
                              font=('Consolas', 9), yscrollcommand=scrollbar.set,
                              state=tk.DISABLED)
        self.console.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        scrollbar.config(command=self.console.yview)
        
    def draw_grid(self):
        """Dibujar grid en el canvas"""
        grid_spacing = 50  # píxeles
        
        # Líneas verticales
        for x in range(0, self.canvas_width + 1, grid_spacing):
            self.canvas.create_line(x, 0, x, self.canvas_height, fill=self.grid_color, width=1)
        
        # Líneas horizontales
        for y in range(0, self.canvas_height + 1, grid_spacing):
            self.canvas.create_line(0, y, self.canvas_width, y, fill=self.grid_color, width=1)
        
        # Origen CNC (esquina INFERIOR izquierda del canvas)
        origin_y = self.canvas_height - 5  # 5 píxeles desde abajo
        self.canvas.create_oval(2, origin_y - 3, 8, origin_y + 3, fill='#ff0000', outline='#ff0000')
        self.canvas.create_text(15, origin_y, text="(0,0) CNC", fill='#ff0000', font=('Arial', 8, 'bold'))
        
        # Indicadores de ejes
        # Eje X (derecha)
        self.canvas.create_line(10, origin_y, 40, origin_y, fill='#00ff00', width=2, arrow=tk.LAST)
        self.canvas.create_text(45, origin_y, text="X+", fill='#00ff00', font=('Arial', 8, 'bold'))
        
        # Eje Y (arriba desde el origen)
        self.canvas.create_line(5, origin_y, 5, origin_y - 30, fill='#0000ff', width=2, arrow=tk.LAST)
        self.canvas.create_text(5, origin_y - 35, text="Y+", fill='#0000ff', font=('Arial', 8, 'bold'))

    
    def update_ports(self):
        """Actualizar lista de puertos seriales disponibles"""
        ports = [port.device for port in serial.tools.list_ports.comports()]
        self.port_combo['values'] = ports
        if ports:
            self.port_combo.current(0)
            self.log("✓ Puertos detectados: " + ", ".join(ports))
        else:
            self.log("⚠ No se detectaron puertos COM")
    
    def toggle_connection(self):
        """Conectar/Desconectar del CNC"""
        if not self.is_connected:
            try:
                port = self.port_var.get()
                if not port:
                    messagebox.showerror("Error", "Selecciona un puerto COM")
                    return
                
                self.serial_port = serial.Serial(port, 115200, timeout=1)
                time.sleep(2)  # Esperar inicialización del ESP32
                
                self.is_connected = True
                self.btn_connect.config(text="🔌 Desconectar", bg='#7a2d2d')
                self.lbl_status.config(text="✅ Conectado", fg='#66ff66')
                self.btn_draw.config(state=tk.NORMAL)
                self.log(f"✓ Conectado a {port}")
                
                # Iniciar hilo de lectura
                threading.Thread(target=self.read_serial, daemon=True).start()
                
            except Exception as e:
                messagebox.showerror("Error de Conexión", f"No se pudo conectar:\n{str(e)}")
                self.log(f"✗ Error: {str(e)}")
        else:
            if self.serial_port:
                self.serial_port.close()
            self.is_connected = False
            self.btn_connect.config(text="🔌 Conectar", bg='#2d7a2d')
            self.lbl_status.config(text="⭕ Desconectado", fg='#ff6666')
            self.btn_draw.config(state=tk.DISABLED)
            self.log("✓ Desconectado")
    
    def send_command(self, command):
        """Enviar comando al CNC"""
        if self.serial_port and self.is_connected:
            try:
                self.serial_port.write((command + '\n').encode())
                self.log(f"→ Enviado: {command}")
                return True
            except Exception as e:
                self.log(f"✗ Error al enviar: {str(e)}")
                return False
        return False
    
    def read_serial(self):
        """Leer respuestas del CNC (hilo secundario)"""
        while self.is_connected:
            try:
                if self.serial_port.in_waiting:
                    line = self.serial_port.readline().decode('utf-8', errors='ignore').strip()
                    if line:
                        self.log(f"← {line}")
            except:
                pass
            time.sleep(0.1)
    
    def log(self, message):
        """Agregar mensaje a la consola"""
        self.console.config(state=tk.NORMAL)
        self.console.insert(tk.END, message + '\n')
        self.console.see(tk.END)
        self.console.config(state=tk.DISABLED)
    
    def start_draw(self, event):
        """Iniciar trazo con el mouse"""
        self.drawing_points.append([])  # Nueva línea
        x_mm, y_mm = self.pixel_to_mm(event.x, event.y)
        self.drawing_points[-1].append((event.x, event.y, x_mm, y_mm))
    
    def draw(self, event):
        """Dibujar mientras se arrastra el mouse"""
        if len(self.drawing_points) > 0:
            last_point = self.drawing_points[-1][-1]
            x_mm, y_mm = self.pixel_to_mm(event.x, event.y)
            
            # Dibujar línea en el canvas
            self.canvas.create_line(last_point[0], last_point[1], event.x, event.y,
                                   fill=self.draw_color, width=2, capstyle=tk.ROUND)
            
            self.drawing_points[-1].append((event.x, event.y, x_mm, y_mm))
    
    def end_draw(self, event):
        """Terminar trazo"""
        pass
    
    def pixel_to_mm(self, px, py):
        """Convertir coordenadas de píxeles a milímetros
        
        IMPORTANTE: El canvas tiene origen en esquina SUPERIOR izquierda (0,0)
        El CNC tiene origen en esquina INFERIOR izquierda (0,0)
        Por lo tanto, invertimos el eje Y
        """
        x_mm = px / self.scale_factor
        # Invertir Y: canvas Y=0 (arriba) -> CNC Y=max (arriba en físico)
        y_mm = (self.canvas_height - py) / self.scale_factor
        return x_mm, y_mm
    
    def mm_to_steps(self, mm):
        """Convertir milímetros a pasos de motor"""
        return int(mm * self.steps_per_mm)
    
    def update_mouse_position(self, event):
        """Actualizar posición del mouse en tiempo real"""
        x_mm, y_mm = self.pixel_to_mm(event.x, event.y)
        self.lbl_mouse_pos.config(text=f"Mouse: ({x_mm:.1f}, {y_mm:.1f}) mm")
    
    def clear_canvas(self):
        """Limpiar el canvas"""
        self.canvas.delete('all')
        self.draw_grid()
        self.drawing_points = []
        self.log("✓ Canvas limpiado")
    
    def go_home(self):
        """Enviar comando Home al CNC"""
        if self.send_command('H'):
            self.current_pos = [0, 0]
            self.update_cnc_position()
    
    def pen_up(self):
        """Subir el lápiz"""
        if self.send_command('U'):
            self.pen_is_down = False
    
    def pen_down(self):
        """Bajar el lápiz"""
        if self.send_command('B'):
            self.pen_is_down = True
    
    def update_cnc_position(self):
        """Actualizar display de posición CNC"""
        self.lbl_cnc_pos.config(text=f"CNC: ({self.current_pos[0]}, {self.current_pos[1]}) pasos")
    
    def send_drawing(self):
        """Enviar el dibujo al CNC"""
        if not self.drawing_points:
            messagebox.showwarning("Sin Dibujo", "No hay nada para dibujar")
            return
        
        if not self.is_connected:
            messagebox.showerror("Error", "Conecta al CNC primero")
            return
        
        self.is_drawing = True
        self.btn_draw.config(state=tk.DISABLED)
        self.btn_pause.config(state=tk.NORMAL)
        
        # Ejecutar en hilo separado para no bloquear la UI
        threading.Thread(target=self.draw_on_cnc, daemon=True).start()
    
    def draw_on_cnc(self):
        """Proceso de dibujo en el CNC (hilo separado)"""
        try:
            self.log("🎨 Iniciando dibujo...")
            
            # Ir a home primero
            self.send_command('H')
            time.sleep(2)
            
            total_lines = len(self.drawing_points)
            current_line = 0
            
            for line in self.drawing_points:
                if not self.is_drawing:
                    self.log("⏸️ Dibujo pausado")
                    break
                
                current_line += 1
                self.progress['value'] = (current_line / total_lines) * 100
                self.lbl_progress.config(text=f"{current_line} / {total_lines} líneas")
                
                # Primer punto de la línea - ir sin dibujar
                first_point = line[0]
                x_steps = self.mm_to_steps(first_point[2])
                y_steps = self.mm_to_steps(first_point[3])
                
                # Mover a posición inicial de la línea
                delta_x = x_steps - self.current_pos[0]
                delta_y = y_steps - self.current_pos[1]
                
                if delta_x != 0:
                    self.send_command(f"X{delta_x}")
                    time.sleep(abs(delta_x) * 0.002)  # 2ms por paso
                
                if delta_y != 0:
                    self.send_command(f"Y{delta_y}")
                    time.sleep(abs(delta_y) * 0.002)
                
                self.current_pos = [x_steps, y_steps]
                
                # Bajar lápiz
                self.send_command('B')
                time.sleep(0.3)
                
                # Dibujar resto de puntos
                for point in line[1:]:
                    if not self.is_drawing:
                        break
                    
                    x_steps = self.mm_to_steps(point[2])
                    y_steps = self.mm_to_steps(point[3])
                    
                    delta_x = x_steps - self.current_pos[0]
                    delta_y = y_steps - self.current_pos[1]
                    
                    if delta_x != 0:
                        self.send_command(f"X{delta_x}")
                        time.sleep(abs(delta_x) * 0.002)
                    
                    if delta_y != 0:
                        self.send_command(f"Y{delta_y}")
                        time.sleep(abs(delta_y) * 0.002)
                    
                    self.current_pos = [x_steps, y_steps]
                    self.update_cnc_position()
                
                # Subir lápiz al terminar línea
                self.send_command('U')
                time.sleep(0.3)
            
            self.log("✓ Dibujo completado!")
            messagebox.showinfo("Completado", "¡Dibujo finalizado!")
            
        except Exception as e:
            self.log(f"✗ Error durante el dibujo: {str(e)}")
            messagebox.showerror("Error", f"Error durante el dibujo:\n{str(e)}")
        
        finally:
            self.is_drawing = False
            self.btn_draw.config(state=tk.NORMAL)
            self.btn_pause.config(state=tk.DISABLED)
            self.progress['value'] = 0
    
    def pause_drawing(self):
        """Pausar/Reanudar el dibujo"""
        self.is_drawing = not self.is_drawing
        if self.is_drawing:
            self.btn_pause.config(text="⏸️ PAUSAR")
        else:
            self.btn_pause.config(text="▶️ REANUDAR")
    
    def save_drawing(self):
        """Guardar dibujo a archivo JSON"""
        if not self.drawing_points:
            messagebox.showwarning("Sin Dibujo", "No hay nada para guardar")
            return
        
        filename = filedialog.asksaveasfilename(
            defaultextension=".json",
            filetypes=[("JSON files", "*.json"), ("All files", "*.*")]
        )
        
        if filename:
            try:
                with open(filename, 'w') as f:
                    json.dump(self.drawing_points, f)
                self.log(f"✓ Dibujo guardado en: {filename}")
                messagebox.showinfo("Guardado", "Dibujo guardado exitosamente")
            except Exception as e:
                messagebox.showerror("Error", f"No se pudo guardar:\n{str(e)}")
    
    def load_drawing(self):
        """Cargar dibujo desde archivo JSON"""
        filename = filedialog.askopenfilename(
            filetypes=[("JSON files", "*.json"), ("All files", "*.*")]
        )
        
        if filename:
            try:
                with open(filename, 'r') as f:
                    self.drawing_points = json.load(f)
                
                # Redibujar en canvas
                self.clear_canvas()
                for line in self.drawing_points:
                    for i in range(len(line) - 1):
                        x1, y1 = line[i][0], line[i][1]
                        x2, y2 = line[i+1][0], line[i+1][1]
                        self.canvas.create_line(x1, y1, x2, y2, fill=self.draw_color, width=2)
                
                self.log(f"✓ Dibujo cargado desde: {filename}")
                messagebox.showinfo("Cargado", "Dibujo cargado exitosamente")
            except Exception as e:
                messagebox.showerror("Error", f"No se pudo cargar:\n{str(e)}")
    
    def update_config(self):
        """Actualizar configuración de pasos/mm"""
        try:
            new_steps = float(self.entry_steps.get())
            self.steps_per_mm = new_steps
            self.log(f"✓ Configuración actualizada: {new_steps} pasos/mm")
            messagebox.showinfo("Actualizado", f"Pasos/mm actualizado a: {new_steps}")
        except ValueError:
            messagebox.showerror("Error", "Valor inválido para pasos/mm")
    
    def show_calibration_window(self):
        """Mostrar ventana de calibración"""
        cal_window = tk.Toplevel(self.root)
        cal_window.title("⚙️ Asistente de Calibración")
        cal_window.geometry("400x500")
        cal_window.configure(bg=self.bg_color)
        
        tk.Label(cal_window, text="🎯 Calibración del CNC", bg=self.bg_color, fg=self.fg_color,
                font=('Arial', 14, 'bold')).pack(pady=20)
        
        tk.Label(cal_window, text="Este asistente te ayudará a calibrar\ntu CNC para dibujos precisos",
                bg=self.bg_color, fg=self.fg_color, font=('Arial', 10)).pack(pady=10)
        
        # Botones de calibración
        btn_frame = tk.Frame(cal_window, bg=self.bg_color)
        btn_frame.pack(pady=20, padx=20, fill=tk.X)
        
        tk.Button(btn_frame, text="📐 Calibrar Eje X", command=lambda: self.send_command('C'),
                 bg='#4a7a9d', fg=self.fg_color, font=('Arial', 11, 'bold'), height=2).pack(fill=tk.X, pady=5)
        
        tk.Button(btn_frame, text="📐 Calibrar Eje Y", command=lambda: self.send_command('D'),
                 bg='#4a7a9d', fg=self.fg_color, font=('Arial', 11, 'bold'), height=2).pack(fill=tk.X, pady=5)
        
        tk.Button(btn_frame, text="🧪 Test 4 Direcciones", command=lambda: self.send_command('A'),
                 bg='#9d7a4a', fg=self.fg_color, font=('Arial', 11, 'bold'), height=2).pack(fill=tk.X, pady=5)
        
        tk.Button(btn_frame, text="📊 Ver Datos IMU", command=lambda: self.send_command('I'),
                 bg='#7a4a9d', fg=self.fg_color, font=('Arial', 11, 'bold'), height=2).pack(fill=tk.X, pady=5)
        
        ttk.Separator(cal_window, orient=tk.HORIZONTAL).pack(fill=tk.X, pady=15)
        
        tk.Label(cal_window, text="📝 Instrucciones:", bg=self.bg_color, fg=self.fg_color,
                font=('Arial', 11, 'bold')).pack()
        
        instructions = """
1. Calibrar Eje X: Encuentra límites
2. Calibrar Eje Y: Encuentra límites
3. Test 4 Direcciones: Verifica movimiento
4. Mide el área real de trabajo en mm
5. Actualiza Pasos/mm en la configuración
        """
        
        tk.Label(cal_window, text=instructions, bg=self.bg_color, fg=self.fg_color,
                font=('Arial', 9), justify=tk.LEFT).pack(pady=10)

def main():
    root = tk.Tk()
    app = CNCPlotterGUI(root)
    root.mainloop()

if __name__ == "__main__":
    main()
