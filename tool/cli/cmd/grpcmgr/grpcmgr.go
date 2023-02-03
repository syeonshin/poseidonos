package grpcmgr

import (
	"cli/cmd/globals"
	"context"
	"errors"
	"fmt"
	"pnconnector/src/log"
	"time"

    "kouros"
    "kouros/pos"
	pb "kouros/api"

	"google.golang.org/grpc"
)

const dialErrorMsg = "Could not connect to the CLI server. Is PoseidonOS running?"
const dialTimeout = 10

// TODO (mj): We temporarily set long timeout values for mount/unmount array commands.
const (
	unmountArrayCmdTimeout uint32 = 1800
	mountArrayCmdTimeout   uint32 = 600
)

func GetPOSManager() (pos.POSManager, error) {
    posMngr, err := kouros.NewPOSManager(pos.GRPC)
    if err != nil {
        return nil, err
    }

    nodeName := globals.NodeName
    gRpcServerAddress := globals.GrpcServerAddress

    if nodeName != "" {
        var err error
        gRpcServerAddress, err = GetIpv4(nodeName)
        if err != nil {
            return nil, errors.New("an error occured while getting the ipv4 address of a node: " + err.Error())
        }
    }

    posMngr.Init("cli", gRpcServerAddress)
    return posMngr, err
}

func dialToCliServer() (*grpc.ClientConn, error) {
	nodeName := globals.NodeName
	gRpcServerAddress := globals.GrpcServerAddress

	if nodeName != "" {
		var err error
		gRpcServerAddress, err = GetIpv4(nodeName)
		if err != nil {
			return nil, errors.New("an error occured while getting the ipv4 address of a node: " + err.Error())
		}
	}

	conn, err := grpc.Dial(gRpcServerAddress, grpc.WithTimeout(time.Second*dialTimeout), grpc.WithInsecure(), grpc.WithBlock())
	return conn, err
}

func SendStartTelemetryRpc(req *pb.StartTelemetryRequest) (*pb.StartTelemetryResponse, error) {
	conn, err := dialToCliServer()
	if err != nil {
		err := errors.New(fmt.Sprintf("%s (internal error message: %s)",
			dialErrorMsg, err.Error()))
		return nil, err
	}
	defer conn.Close()

	c := pb.NewPosCliClient(conn)
	ctx, cancel := context.WithTimeout(context.Background(), time.Second*time.Duration(globals.ReqTimeout))
	defer cancel()

	res, err := c.StartTelemetry(ctx, req)

	if err != nil {
		log.Error("error: ", err.Error())
		return nil, err
	}

	return res, err
}

func SendStopTelemetryRpc(req *pb.StopTelemetryRequest) (*pb.StopTelemetryResponse, error) {
	conn, err := dialToCliServer()
	if err != nil {
		err := errors.New(fmt.Sprintf("%s (internal error message: %s)",
			dialErrorMsg, err.Error()))
		return nil, err
	}
	defer conn.Close()

	c := pb.NewPosCliClient(conn)
	ctx, cancel := context.WithTimeout(context.Background(), time.Second*time.Duration(globals.ReqTimeout))
	defer cancel()

	res, err := c.StopTelemetry(ctx, req)

	if err != nil {
		log.Error("error: ", err.Error())
		return nil, err
	}

	return res, err
}

func SendSetTelemetryPropertyRpc(req *pb.SetTelemetryPropertyRequest) (*pb.SetTelemetryPropertyResponse, error) {
	conn, err := dialToCliServer()
	if err != nil {
		err := errors.New(fmt.Sprintf("%s (internal error message: %s)",
			dialErrorMsg, err.Error()))
		return nil, err
	}
	defer conn.Close()

	c := pb.NewPosCliClient(conn)
	ctx, cancel := context.WithTimeout(context.Background(), time.Second*time.Duration(globals.ReqTimeout))
	defer cancel()

	res, err := c.SetTelemetryProperty(ctx, req)

	if err != nil {
		log.Error("error: ", err.Error())
		return nil, err
	}

	return res, err
}

func SendGetTelemetryProperty(req *pb.GetTelemetryPropertyRequest) (*pb.GetTelemetryPropertyResponse, error) {
	conn, err := dialToCliServer()
	if err != nil {
		err := errors.New(fmt.Sprintf("%s (internal error message: %s)",
			dialErrorMsg, err.Error()))
		return nil, err
	}
	defer conn.Close()

	c := pb.NewPosCliClient(conn)
	ctx, cancel := context.WithTimeout(context.Background(), time.Second*time.Duration(globals.ReqTimeout))
	defer cancel()

	res, err := c.GetTelemetryProperty(ctx, req)

	if err != nil {
		log.Error("error: ", err.Error())
		return nil, err
	}

	return res, err
}

func SendCreateSubsystem(req *pb.CreateSubsystemRequest) (*pb.CreateSubsystemResponse, error) {
	conn, err := dialToCliServer()
	if err != nil {
		err := errors.New(fmt.Sprintf("%s (internal error message: %s)",
			dialErrorMsg, err.Error()))
		return nil, err
	}
	defer conn.Close()

	c := pb.NewPosCliClient(conn)
	ctx, cancel := context.WithTimeout(context.Background(), time.Second*time.Duration(globals.ReqTimeout))
	defer cancel()

	res, err := c.CreateSubsystem(ctx, req)
	if err != nil {
		log.Error("error: ", err.Error())
		return nil, err
	}

	return res, err
}

func SendDeleteSubsystem(req *pb.DeleteSubsystemRequest) (*pb.DeleteSubsystemResponse, error) {
	conn, err := dialToCliServer()
	if err != nil {
		err := errors.New(fmt.Sprintf("%s (internal error message: %s)",
			dialErrorMsg, err.Error()))
		return nil, err
	}
	defer conn.Close()

	c := pb.NewPosCliClient(conn)
	ctx, cancel := context.WithTimeout(context.Background(), time.Second*time.Duration(globals.ReqTimeout))
	defer cancel()

	res, err := c.DeleteSubsystem(ctx, req)
	if err != nil {
		log.Error("error: ", err.Error())
		return nil, err
	}

	return res, err
}

func SendAddListener(req *pb.AddListenerRequest) (*pb.AddListenerResponse, error) {
	conn, err := dialToCliServer()
	if err != nil {
		err := errors.New(fmt.Sprintf("%s (internal error message: %s)",
			dialErrorMsg, err.Error()))
		return nil, err
	}
	defer conn.Close()

	c := pb.NewPosCliClient(conn)
	ctx, cancel := context.WithTimeout(context.Background(), time.Second*time.Duration(globals.ReqTimeout))
	defer cancel()

	res, err := c.AddListener(ctx, req)
	if err != nil {
		log.Error("error: ", err.Error())
		return nil, err
	}

	return res, err
}

func SendListSubsystem(req *pb.ListSubsystemRequest) (*pb.ListSubsystemResponse, error) {
	conn, err := dialToCliServer()
	if err != nil {
		err := errors.New(fmt.Sprintf("%s (internal error message: %s)",
			dialErrorMsg, err.Error()))
		return nil, err
	}
	defer conn.Close()

	c := pb.NewPosCliClient(conn)
	ctx, cancel := context.WithTimeout(context.Background(), time.Second*time.Duration(globals.ReqTimeout))
	defer cancel()

	res, err := c.ListSubsystem(ctx, req)
	if err != nil {
		log.Error("error: ", err.Error())
		return nil, err
	}

	return res, err
}

func SendSubsystemInfo(req *pb.SubsystemInfoRequest) (*pb.SubsystemInfoResponse, error) {
	conn, err := dialToCliServer()
	if err != nil {
		err := errors.New(fmt.Sprintf("%s (internal error message: %s)",
			dialErrorMsg, err.Error()))
		return nil, err
	}
	defer conn.Close()

	c := pb.NewPosCliClient(conn)
	ctx, cancel := context.WithTimeout(context.Background(), time.Second*time.Duration(globals.ReqTimeout))
	defer cancel()

	res, err := c.SubsystemInfo(ctx, req)
	if err != nil {
		log.Error("error: ", err.Error())
		return nil, err
	}

	return res, err
}

func SendCreateTransport(req *pb.CreateTransportRequest) (*pb.CreateTransportResponse, error) {
	conn, err := dialToCliServer()
	if err != nil {
		err := errors.New(fmt.Sprintf("%s (internal error message: %s)",
			dialErrorMsg, err.Error()))
		return nil, err
	}
	defer conn.Close()

	c := pb.NewPosCliClient(conn)
	ctx, cancel := context.WithTimeout(context.Background(), time.Second*time.Duration(globals.ReqTimeout))
	defer cancel()

	res, err := c.CreateTransport(ctx, req)
	if err != nil {
		log.Error("error: ", err.Error())
		return nil, err
	}

	return res, err
}

func SendMountVolume(req *pb.MountVolumeRequest) (*pb.MountVolumeResponse, error) {
	conn, err := dialToCliServer()
	if err != nil {
		err := errors.New(fmt.Sprintf("%s (internal error message: %s)",
			dialErrorMsg, err.Error()))
		return nil, err
	}
	defer conn.Close()

	c := pb.NewPosCliClient(conn)
	ctx, cancel := context.WithTimeout(context.Background(), time.Second*time.Duration(globals.ReqTimeout))
	defer cancel()

	res, err := c.MountVolume(ctx, req)
	if err != nil {
		log.Error("error: ", err.Error())
		return nil, err
	}

	return res, err
}

func SendListWBT(req *pb.ListWBTRequest) (*pb.ListWBTResponse, error) {
	conn, err := dialToCliServer()
	if err != nil {
		log.Error(err)
		errToReturn := errors.New(dialErrorMsg)
		return nil, errToReturn
	}
	defer conn.Close()

	c := pb.NewPosCliClient(conn)
	ctx, cancel := context.WithTimeout(context.Background(), time.Second*time.Duration(globals.ReqTimeout))
	defer cancel()

	res, err := c.ListWBT(ctx, req)
	if err != nil {
		log.Error("error: ", err.Error())
		return nil, err
	}

	return res, err
}

func SendWBT(req *pb.WBTRequest) (*pb.WBTResponse, error) {
	conn, err := dialToCliServer()
	if err != nil {
		log.Error(err)
		errToReturn := errors.New(dialErrorMsg)
		return nil, errToReturn
	}
	defer conn.Close()

	c := pb.NewPosCliClient(conn)
	ctx, cancel := context.WithTimeout(context.Background(), time.Second*time.Duration(globals.ReqTimeout))
	defer cancel()

	res, err := c.WBT(ctx, req)
	if err != nil {
		log.Error("error: ", err.Error())
		return nil, err
	}

	return res, err
}
