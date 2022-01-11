`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    18:34:22 05/26/2021 
// Design Name: 
// Module Name:    advancedpipeline 
// Project Name: 
// Target Devices: 
// Tool versions: 
// Description: 
//
// Dependencies: 
//
// Revision: 
// Revision 0.01 - File Created
// Additional Comments: 
//
//////////////////////////////////////////////////////////////////////////////////
module advancedpipeline(clk_operating, reset, result, zero
    );
	 
	 input clk_operating, reset;
	 output [31:0] result;
	 output zero;
	 //IF
	 wire clk, pcwrite, branchcontrol, ifmuxcontrol, branchsignal, xorbranch;
	 wire [31:0] ifmuxout, pcvalue, pc, instruction, pcpfour, branchaddr, jumpaddr;
	 //IF/ID Latch
	 wire ifflushcontrol, ifflush1, iflctrl, idflush1;
	 wire [31:0] idpcpf, idinstruction;
	 //ID
	 wire idflush2,jmux;
	 wire [7:0] wbmex;
	 wire [31:0] reg1read, slldata, reg2read, wdata, extendedinstrc;
	 wire [4:0] exinstrc2016, exinstrc2521, exinstrc1511, wregaddr;
	 //ID/EX Latch
	 wire idmuxcontrol, exjmux;
	 wire [31:0] expcpf, exrdata1, exrdata2, eximmediate;
	 wire [1:0] idexwb, idexm, exwb, exm;
	 wire [3:0] idexex, exex;
	 //EX
	 wire [31:0] exmemaluresult, aluinput1, aluinput2, alumuxinput;
	 wire [1:0] data1muxcontrol, data2muxcontrol;
	 wire [2:0] alucontrolop;
	 wire [4:0] exmemregdest, wregdest;
	 //EX/MEM Latch
	 wire [31:0] mempcpf, datamemwdata, dmemrdata;
	 wire memjmux;
	 wire [1:0] memwbwb, memm;
	 //MEM/WB Latch
	 wire [31:0] wbpcpf, wbdmemrdata,wbaluresult;
	 wire wbjmux;
	 wire [1:0] wbwb;
	 wire [4:0] memwbregdest1;
	 //WB
	 wire [31:0] wdata2;
	 clockdivider clkdiv(clk_operating, reset, clk);
	 //IF Stage
	 latch pcblock[31:0](clk, ifmuxout, pcvalue, pcwrite, reset);
	 programcounter pcmodule(pcvalue, pc, reset);
	 instrMemory instructionmemory(pc, instruction, reset);
	 add pcadder(pcpfour, pc, 4);
	 ifmux ifstagemux(branchaddr, jumpaddr, pcpfour, branchcontrol, ifmuxcontrol, ifmuxout);
	 branchand bandgate(branchsignal, xorbranch, branchcontrol);
	 //IF/ID Latch
	 orgate ifflushor(ifflushcontrol, ifflush1, branchcontrol);
	 iflatch ifidlpcpf[31:0](clk, pcpfour, idpcpf, iflctrl, ifflushcontrol, reset);
	 iflatch ifidinstruction[31:0](clk,instruction,idinstruction, iflctrl, ifflushcontrol, reset);
	 latch ifflushlatch(clk, ifflushcontrol, idflush1, 1'b1, reset);
	 //ID
	 jumpoperator jump(idinstruction, idpcpf, reg1read, jumpaddr);
	 hazarddection hzrddetect(exinstrc2016, idinstruction, exm[0], idflush2, iflctrl, pcwrite);
	 add idadder(branchaddr, idpcpf, slldata);
	 signextend extender(idinstruction[15:0], extendedinstrc);
	 sll shiftleft(extendedinstrc, slldata);
	 registers registersource(clk, idinstruction[25:21], idinstruction[20:16], wregaddr,wdata, reg1read, reg2read, wbwb[1], reset);
	 xorgate idxorgate(reg1read, reg2read, xorbranch);
	 control maincontrol(ifmuxcontrol, branchsignal, ifflush1, wbmex, idinstruction, jmux);
	 //ID/EX Latch
	 orgate idflushor(idmuxcontrol, idflush1, idflush2);
	 idmux idcontrolmux(wbmex, 8'b0, idmuxcontrol, idexwb, idexm, idexex);
	 latch idexpcpf[31:0](clk, idpcpf, expcpf, 1'b1, reset);
	 latch idexjumpctrl(clk, jmux, exjmux, 1'b1, reset);
	 latch idexwblatch[1:0](clk, idexwb, exwb, 1'b1,reset);
	 latch idexmlatch[1:0](clk, idexm, exm, 1'b1, reset);
	 latch idexexlatch[3:0](clk, idexex, exex, 1'b1, reset);
	 latch idexrdata1[31:0](clk, reg1read, exrdata1, 1'b1, reset);
	 latch idexrdata2[31:0](clk, reg2read, exrdata2, 1'b1, reset);
	 latch idexsextend[31:0](clk, extendedinstrc, eximmediate, 1'b1, reset);
	 latch idexintr2521[4:0](clk, idinstruction[25:21], exinstrc2521, 1'b1, reset);
	 latch idexintr2016[4:0](clk, idinstruction[20:16], exinstrc2016, 1'b1, reset);
	 latch idexintr1511[4:0](clk, idinstruction[15:11], exinstrc1511, 1'b1, reset);
	 //EX
	 mux_5bit regdestmux(exinstrc2016,exinstrc1511,exex[0],wregdest);
	 tinput_mux_32bit tripinput1(exrdata1, exmemaluresult, wdata, data1muxcontrol, aluinput1);
	 tinput_mux_32bit tripinput2(exrdata2, exmemaluresult, wdata, data2muxcontrol, alumuxinput);
	 mux_32bit alumux(alumuxinput,eximmediate,exex[3],aluinput2);
	 alucontrol controlsofalu(exex[2:1], alucontrolop);
	 alu exalu(aluinput1, aluinput2, alucontrolop, zero, result);
	 forwarding_unit fwunit(exinstrc2521,exinstrc2016,exmemregdest,memwbregdest1,memwbwb[1],wbwb[1], data1muxcontrol, data2muxcontrol);

	 //EX/MEM Latch
	 latch exmempcpf[31:0](clk, expcpf, mempcpf, 1'b1, reset);
	 latch exmemjumpctrl(clk, exjmux, memjmux, 1'b1, reset);
	 latch exmemwblatch[1:0](clk, exwb, memwbwb, 1'b1, reset);
	 latch exmemmlatch[1:0](clk, exm, memm, 1'b1, reset);
	 latch exmemalures[31:0](clk, result, exmemaluresult, 1'b1, reset);
	 latch exmemwdata[31:0](clk, alumuxinput, datamemwdata, 1'b1, reset);
	 latch exmemregdesto[4:0](clk, wregdest, exmemregdest, 1'b1, reset);
	 //MEM
	 dataMemory datamemory(exmemaluresult, memm[0], memm[1], datamemwdata, dmemrdata, reset);
	 //MEM/WB Latch
	 latch memwbpcpf[31:0](clk, mempcpf, wbpcpf, 1'b1, reset);
	 latch memwbjumpctrl(clk, memjmux, wbjmux,1'b1, reset);
	 latch memwbwblatch[1:0](clk, memwbwb, wbwb, 1'b1, reset);
	 latch datamemlatch[31:0](clk, dmemrdata, wbdmemrdata, 1'b1, reset);
	 latch wbaluresultlatch[31:0](clk, exmemaluresult, wbaluresult, 1'b1, reset);
	 latch wbregdest[4:0](clk, exmemregdest, memwbregdest1, 1'b1, reset);
	 //WB
	 mux_32bit wbmux(wbdmemrdata, wbaluresult, wbwb[0], wdata2);
	 mux_32bit jumpwbmux(wdata2, wbpcpf, wbjmux, wdata);
	 mux_5bit jumpdaddrmux(memwbregdest1, 5'b11111, wbjmux, wregaddr);
endmodule

module jumpoperator(instruction, pcpf, jrreaddata, jumpaddr);
input [31:0] instruction, pcpf, jrreaddata;
output reg [31:0] jumpaddr;
reg [31:0] tmp,tmp1,tmp2;

always@(*) begin
if(instruction[31:26] == 6'b000000) begin
jumpaddr = jrreaddata;
end
if(instruction[31:26] == 6'b000011) begin
tmp = (pcpf[31:0] && 32'hf0000000);
tmp1 = (instruction[25:0]^32'b0);
tmp2 = tmp1<<2;
jumpaddr = (tmp||tmp2);
end

end
endmodule


module branchand(branchsignal, xorbranch, branchcontrol);
input branchsignal, xorbranch;
output reg branchcontrol;

always@(*) begin
if(branchsignal && xorbranch == 1) begin
branchcontrol <= 1;
end
else branchcontrol <= 0;
end
endmodule

module hazarddection(lwrt, instruction, memread, ifflush2, ifidwrite, pcwrite);
input [4:0] lwrt;
input [31:0] instruction;
input memread;
output reg ifflush2, ifidwrite, pcwrite;
always@(*) begin
if(((lwrt == instruction[25:21]) || (lwrt == instruction[20:16])) && (memread == 1)) begin
ifflush2 <= 1'b1;
ifidwrite <= 1'b0;
pcwrite <= 1'b0;
end
else begin
ifflush2 <= 1'b0;
ifidwrite <= 1'b1;
pcwrite <= 1'b1;
end
end
endmodule


module idmux(wbmex, zeroinput, idmuxcontrol, idexwb, idexm, idexex);
input [7:0] wbmex, zeroinput;
input idmuxcontrol;
output reg [1:0] idexwb, idexm;
output reg [3:0] idexex;

always@(*) begin
if(idmuxcontrol == 0) begin
idexwb <= wbmex[7:6];
idexm <= wbmex[5:4];
idexex <= wbmex[3:0];
end
else if(idmuxcontrol == 1) begin
idexwb <= 2'b0;
idexm <= 2'b0;
idexex <= 4'b0;
end
else begin
idexwb <= 2'bx;
idexm <= 2'bx;
idexex <= 4'bx;
end
end
endmodule

module forwarding_unit(rs,rt,exmemrd,memwbrd,exmemwb,memwbwb, data1muxcontrol, data2muxcontrol);
input [4:0] rs,rt,exmemrd,memwbrd;
input exmemwb, memwbwb;
output reg[1:0] data1muxcontrol, data2muxcontrol;

always@(*) begin
if(((exmemrd==rs)||(exmemrd==rt))&&(exmemwb)) begin
if(exmemrd==rs) begin
data1muxcontrol <= 2'b01;
end
if(exmemrd==rt) begin
data2muxcontrol <= 2'b01;
end
if((exmemrd == rs) && (memwbrd == rt) &&(exmemrd!=rt)&&(memwbwb)) begin
data2muxcontrol <= 2'b10;
end
if((exmemrd == rs) && (memwbrd == rt) &&(exmemrd!=rt)&&(~(memwbwb))) begin
data2muxcontrol <= 2'b00;
end
if((exmemrd == rs) && (memwbrd != rt) &&(exmemrd!=rt)) begin
data2muxcontrol <= 2'b00;
end
if((exmemrd == rt) && (memwbrd == rs) &&(exmemrd!=rs)&&(memwbwb)) begin
data1muxcontrol <= 2'b10;
end
if((exmemrd == rt) && (memwbrd == rs) &&(exmemrd!=rs)&&(~(memwbwb))) begin
data1muxcontrol <= 2'b00;
end
if((exmemrd == rt) && (memwbrd != rs) &&(exmemrd!=rs)) begin
data1muxcontrol <= 2'b00;
end
end

else if(((memwbrd==rs)||(memwbrd==rt))&&(memwbwb)) begin
if(memwbrd == rs) begin
data1muxcontrol <= 2'b10;
end
if(memwbrd == rt) begin
data2muxcontrol <= 2'b10;
end
if((memwbrd == rs) && (memwbrd != rt)) begin
data2muxcontrol <= 2'b00;
end
if((memwbrd == rt) && (memwbrd != rs)) begin
data1muxcontrol <= 2'b00;
end
end
else begin
data1muxcontrol <= 2'b00;
data2muxcontrol <= 2'b00;
end
end
endmodule

module tinput_mux_32bit(a,b,c,op,out);
input [31:0] a,b,c;
input [1:0]op;
output reg[31:0] out;

always@(*) begin
case(op)
2'b00: out <= a;
2'b01: out <= b;
2'b10: out <= c;
default: out<=32'bx;
endcase
end
endmodule

module ifmux(a,b,c,branch,control,out);
input [31:0] a,b,c;
input branch, control;
output reg[31:0] out;

always@(*) begin
if(branch == 1) begin
out <= a;
end
else if(branch!=1 && control == 1) begin
out <= b;
end
else begin
out <=c;
end
end
endmodule

module orgate(ifflushcontrol, ifflush1, ifflush2);
input ifflush1, ifflush2;
output reg ifflushcontrol;
always@(*) begin
if(ifflush1 | ifflush2 ==1) begin
ifflushcontrol <= 1'b1;
end
else begin
ifflushcontrol <= 1'b0;
end
end
endmodule

module latch( clk, in, out, control, reset
    );
	 
	 input clk, in;
	 input reset;
	 input control;
	 output reg out;
	 
	 always@(posedge clk, posedge reset) begin
	 if(reset) begin
	 out <= 1'b0;
	 end
	 else if(control == 0) begin
	 out <= out;
	 end
	 else if(control == 1) begin
	 out <= in;
	 end
	 end
endmodule

module iflatch( clk, in, out, control, flushcontrol, reset
    );
	 
	 input clk, in;
	 input reset;
	 input control;
	 input flushcontrol;
	 output reg out;
	 always@(posedge clk) begin
	 if(flushcontrol) begin
	 out <= 1'b0;
	 end
	 else if(control == 0) begin
	 out <= out;
	 end
	 else if(control == 1) begin
	 out <= in;
	 end
	 end
endmodule

module programcounter(in,out,reset);
input [31:0] in;
input reset;
output reg [31:0] out;
always@(*) begin
	 if (reset) begin
	 out <= 32'b0;
	 end
	 else begin
	 out <= in;
	 end
	 end
endmodule

module control(ifmuxctrl, branch, ifflush, wbmex, instruction, jmux);
input [31:0] instruction;
output reg ifmuxctrl, branch, ifflush, jmux;
output reg [7:0] wbmex;
always@(*) begin
case (instruction[31:26])
6'b 000000: if(instruction[5:0] == 6'b100000) begin //add
				ifmuxctrl <= 1'b0;
				branch <= 1'b0;
				ifflush <= 1'b0;
				wbmex <= 8'b11000101;
				jmux <= 1'b0;
				end
				else if(instruction[5:0] == 6'b100010) begin //sub
				ifmuxctrl <= 1'b0;
				branch <= 1'b0;
				ifflush <= 1'b0;
				wbmex <= 8'b11000111;
				jmux <= 1'b0;
				end
				else if(instruction[5:0] == 6'b100100) begin //and
				ifmuxctrl <= 1'b0;
				branch <= 1'b0;
				ifflush <= 1'b0;
				wbmex <= 8'b11000001;
				jmux <= 1'b0;
				end
				else if(instruction[5:0] == 6'b100101) begin //or
				ifmuxctrl <= 1'b0;
				branch <= 1'b0;
				ifflush <= 1'b0;
				wbmex <= 8'b11000011;
				jmux <= 1'b0;
				end
				else if(instruction[5:0] == 6'b001000) begin//jr
				ifmuxctrl <= 1'b1;
				branch <= 1'b0;
				ifflush <= 1'b1;
				wbmex <= 8'bx;
				jmux <= 1'b0;
				end
				else begin
				ifmuxctrl <= 1'bx;
				branch <= 1'bx;
				ifflush <= 1'bx;
				wbmex <= 8'bx;
				jmux <= 1'bx;
				end
6'b 100011: begin //lw
				ifmuxctrl <= 1'b0;
				branch <= 1'b0;
				ifflush <= 1'b0;
				wbmex <= 8'b10011100;
				jmux <= 1'b0;
				end
6'b 101011: begin //sw
				ifmuxctrl <= 1'b0;
				branch <= 1'b0;
				ifflush <= 1'b0;
				wbmex <= 8'b01101100;
				jmux <= 1'b0;
				end
				
6'b 000100: begin //beq
				ifmuxctrl <= 1'bx;
				branch <= 1'b1;
				ifflush <= 1'b0;
				wbmex <= 8'bx;
				jmux <= 1'b0;
				end
				
6'b 000011: begin//jal
				ifmuxctrl <= 1'b1;
				branch <= 1'b0;
				ifflush <= 1'b1;
				wbmex <= 8'b11000000;
				jmux <= 1'b1;
				end
default: begin
			ifmuxctrl <= 1'bx;
			branch <= 1'bx;
			ifflush <= 1'bx;
			wbmex <= 8'bx;
			jmux <= 1'bx;
			end

endcase
end
endmodule

module dataMemory(MemAddr, MemRead, MemWrite, Write_Data, Read_Data, reset);
input [31:0] MemAddr;
input reset;
input MemRead, MemWrite;
input [31:0] Write_Data;
output reg [31:0] Read_Data;

reg [31:0] mem [65:0];

always@(*) begin
if(reset) begin
mem['hc] <= 30;
end
if(MemRead && !MemWrite) begin
Read_Data <= mem[MemAddr[6:0]];
end

else if(!MemRead && MemWrite) begin
mem[MemAddr[6:0]] <= Write_Data;
end

else begin
Read_Data <= 32'bx;
end
end
endmodule

module instrMemory(MemAddr, Read_Data, reset);
input [31:0] MemAddr;
input reset;
output reg [31:0] Read_Data;

reg [31:0] mem [7:0];
reg [3:0] tmp;
always@(*) begin
tmp <= MemAddr[5:2];
if(reset) begin
mem[0] <= 32'b10001100010000010000000000000100;
mem[1] <= 32'b00000000001001010010000000100010;
mem[2] <= 32'b00000000001001110011000000100100;
mem[3] <= 32'b00000000001010010100000000100101;
mem[4] <= 32'b00010000110000001111111111111011;
end
else begin
Read_Data <= mem[tmp];
end
end
endmodule



module add(result, ina, inb);

input [31:0] ina, inb;
output reg[31:0] result;

always@(*) begin
result <= ina + inb;
end

endmodule

module mux_32bit(a,b,op,out);
input [31:0] a,b;
input op;
output reg[31:0] out;

always@(*) begin
case(op)
1'b0: out <= a;
1'b1: out <= b;
default: out<=32'bx;
endcase
end
endmodule

module mux_5bit(a,b,op,out);
input [4:0] a,b;
input op;
output reg[4:0] out;

always@(*) begin
case(op)
1'b0: out <= a;
1'b1: out <= b;
default: out<=5'bx;
endcase
end
endmodule

module signextend(a,out);
input [15:0] a;
output reg[31:0] out;
always@(*) begin
if (a[15] == 0) begin
out <= 32'b0^a;
end
else if (a[15] == 1) begin
out <= 32'hffff0000^a;
end
else begin
out <= 32'bx;
end
end
endmodule

module sll(signextend, out);
input [31:0] signextend;
output reg[31:0] out;

always@(*) begin
out <= signextend<<<2;
end
endmodule

module alu(a, b, op, zero, result);

input [31:0] a, b;
input [2:0] op; //000 and 001 or 010 add 110 subtract 111slt
output reg zero;
output reg [31:0] result;

always@(*) begin
case (op)
3'b000: if(a&b == 0) begin
		  result <= a&b;
		  zero <= 1;
		  end
		  else begin
		  zero <= 0;
		  result <= a&b;		  
		  end
3'b001: if(a|b == 0) begin
		  result <= a|b;
		  zero <= 1;
		  end
		  else begin
		  result <= a|b;
		  zero <= 0;
		  end
3'b010: if(a+b == 0) begin
		  zero <= 1;
		  result <= a+b;
		  end
		  else begin
		  zero <= 0;
		  result <= a+b;
		  end
3'b110: if(a-b == 0) begin
		  zero <= 1;
		  result <= a-b;
		  end
		  else begin
		  zero <= 0;
		  result <= a-b;
		  end
3'b111: if (a<b) begin
		  result <= 32'b1;
		  end
		  else begin
		  result <= 32'b0;
		  end
default: result <= 32'bx;
endcase
end
endmodule

module registers(clk, reg1addr, reg2addr, wregaddr,wdata, reg1read, reg2read, regwrite, reset);
input [4:0] reg1addr, reg2addr, wregaddr;
input [31:0] wdata;
input reset,clk;
input regwrite;
output reg [31:0] reg1read, reg2read;

reg [31:0] register [9:0];


always@(posedge clk, negedge clk, posedge reset) begin
if(reset) begin
register[0] = 32'b0;
register[1] = 32'd20;
register[2] = 32'd8;
register[5] = 32'd2;
register[6] = 32'b0;
register[7] = 32'd1;
register[9] = 32'd3;
end
else if (clk & regwrite == 1) begin
register[wregaddr] = wdata;
end
else begin
reg1read = register[reg1addr];
reg2read = register[reg2addr];
end
end

endmodule

module alucontrol(ex, op);
input [1:0]ex;
output reg [2:0]op;
always@(*) begin
case(ex)
2'b00: op<= 3'b000;
2'b01: op<= 3'b001;
2'b10: op<= 3'b010;
2'b11: op<= 3'b110;
default: op<= 3'bx;
endcase
end
endmodule

module xorgate(rdata1, rdata2, xorbranch);
input [31:0] rdata1, rdata2;
output reg xorbranch;
always@(*) begin
if ( rdata1 [31:0] ^ rdata2 [31:0] == 32'b0) begin
xorbranch <= 1;
end
else xorbranch <=0;
end
endmodule


module clockdivider( clk, rst, clk_operating);
reg [1:0] count;
input clk, rst;
output reg clk_operating;

always @(posedge clk, posedge rst) begin
if(rst) begin
count <= 2'b0;
clk_operating <= 1'b0;
end

else begin
count <= count + 1;
clk_operating <= count[1];
end
end
endmodule

module rinvert(reseta, reset);
input reseta;
output reg reset;
always@(*) begin
reset <= !reseta;
end
endmodule
